// Fill out your copyright notice in the Description page of Project Settings.


#include "MysticMastery/Public/Character/MMCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "MMGameplayTags.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MysticMastery/MysticMastery.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMMCharacterBase::AMMCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = FMMGameplayTags::Get().Debuff_Burn;
	BurnDebuffComponent->Deactivate();

	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunDebuffComponent");
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->DebuffTag = FMMGameplayTags::Get().Debuff_Stun;
	StunDebuffComponent->Deactivate();
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	
	FaceMask = CreateDefaultSubobject<USkeletalMeshComponent>("FaceMask");
	FaceMask->SetupAttachment(GetMesh(), FName("FaceMask"));
	
	//Remove any collision from weapon/Facemask or mesh
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FaceMask->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Set overlap events
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetMesh()->SetGenerateOverlapEvents(false);

	//Collision response to camera
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	FaceMask->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Weapon->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	//Collision response to projectiles
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
}

void AMMCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMMCharacterBase, bIsStunned);
	DOREPLIFETIME(AMMCharacterBase, bIsBurned);
}

void AMMCharacterBase::OnRep_Stunned()
{
	
}

void AMMCharacterBase::OnRep_Burned()
{
	
}

UAbilitySystemComponent* AMMCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AMMCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AMMCharacterBase::Die(const FVector& DeathImpulse)
{
	//Drop weapon, automatically replicated
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));

	//Ragdoll enemy on client and server and activate weapon physics 
	MulticastHandleDeath(DeathImpulse);
}

bool AMMCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AMMCharacterBase::GetAvatar_Implementation()
{
	return this;
}

void AMMCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);

	/* Uncomment this to make the mask to have physics on death */
	//FaceMask->SetSimulatePhysics(true);
	//FaceMask->SetEnableGravity(true);
	//FaceMask->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetSimulatePhysics(false);

	//Disable RVO Avoidance, so dead enemies do not block the path
	UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	CharacterMovementComponent->SetAvoidanceEnabled(false);
	
	Dissolve();
	bDead = true;
	BurnDebuffComponent->Deactivate();
	StunDebuffComponent->Deactivate();
	
	OnDeathDelegate.Broadcast(this);
}

TArray<FTaggedMontage> AMMCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AMMCharacterBase::GetHitEffect_Implementation()
{
	return HitEffect;
}

int32 AMMCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void AMMCharacterBase::IncrementMinionCount_Implementation(const int32 DeltaAmount)
{
	MinionCount += DeltaAmount;
}

ECharacterClass AMMCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegisteredSignature& AMMCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnAscRegistered;
}

FOnDeathSignature& AMMCharacterBase::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

void AMMCharacterBase::KnockBack(const FVector& ForceImpulse)
{
	GetCharacterMovement()->StopMovementImmediately();
	LaunchCharacter(ForceImpulse, true, true);
}

USkeletalMeshComponent* AMMCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

void AMMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMMCharacterBase::StunnedTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	//Immovilize the character
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

/**
 * 
 * @param MontageTag There are only 3 types of Montage Tag according to the weapon they use: "Weapon","LeftHand","RightHand"
 * @return It returns the socket location of the Montage based on the associated tag 
 */
FVector AMMCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	if (MontageTag.MatchesTagExact(FMMGameplayTags::Get().CombatSocket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(FMMGameplayTags::Get().CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(FMMGameplayTags::Get().CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(FMMGameplayTags::Get().CombatSocket_Head))
	{
		return GetMesh()->GetSocketLocation(HeadSocketName);
	}
	return FVector();
}

void AMMCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()) && GameplayEffectClass)
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(
		GameplayEffectClass, Level, EffectContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AMMCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1);
	ApplyEffectToSelf(DefaultVitalAttributes, 1);
}

void AMMCharacterBase::InitializeAbilityActorInfo()
{
}

void AMMCharacterBase::AddCharacterAbilities()
{
	UMMAbilitySystemComponent* ASC = CastChecked<UMMAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;

	//If there is authority, then we must grant the Ability, but its something that the ASC must do
	ASC->AddCharacterAbilities(StartupAbilities);
	ASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

/** Creates a Dynamic material instance out of the MI and swap them */
void AMMCharacterBase::Dissolve()
{
	if (IsValid(CharacterDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DMI = UMaterialInstanceDynamic::Create(CharacterDissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DMI);

		//Start the dissolve animation on blueprints
		StartCharacterDissolveTimeline(DMI);
	}

	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DMI = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DMI);

		//Start the dissolve animation on blueprints
		StartWeaponDissolveTimeline(DMI);
	}
}

FTaggedMontage AMMCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for(FTaggedMontage TaggedMontage : AttackMontages)
	{
		if(TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}
