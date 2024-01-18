// Fill out your copyright notice in the Description page of Project Settings.


#include "MysticMastery/Public/Character/MMCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "MMGameplayTags.h"
#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MysticMastery/MysticMastery.h"

// Sets default values
AMMCharacterBase::AMMCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	FaceMask = CreateDefaultSubobject<USkeletalMeshComponent>("FaceMask");
	FaceMask->SetupAttachment(GetMesh(), FName("FaceMask"));

	//Remove any collision from weapon
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	FaceMask->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FaceMask->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	//Set Basic responses to Camera, and Projectile Channels
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetMesh()->SetGenerateOverlapEvents(false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
}

UAbilitySystemComponent* AMMCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AMMCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AMMCharacterBase::Die()
{
	//Drop weapon, automatically replicated
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));

	//Ragdoll enemy on client and server and activate weapon physics 
	MulticastHandleDeath();
}

bool AMMCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AMMCharacterBase::GetAvatar_Implementation()
{
	return this;
}

void AMMCharacterBase::MulticastHandleDeath_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	FaceMask->SetSimulatePhysics(true);
	FaceMask->SetEnableGravity(true);
	FaceMask->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	bDead = true;
}

TArray<FTaggedMontage> AMMCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AMMCharacterBase::GetHitEffect_Implementation()
{
	return HitEffect;
}

void AMMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

FVector AMMCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const FName SocketName = SocketTagNames.FindChecked(MontageTag);
	
	if (MontageTag.MatchesTagExact(FMMGameplayTags::Get().CombatSocket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(SocketName);
	}
	return GetMesh()->GetSocketLocation(SocketName);
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
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}
