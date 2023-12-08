// Fill out your copyright notice in the Description page of Project Settings.


#include "MysticMastery/Public/Character/MMCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "MysticMastery/MysticMastery.h"

// Sets default values
AMMCharacterBase::AMMCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));

	//Remove any collision from weapon
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
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

void AMMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

FVector AMMCharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AMMCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const 
{
	check(IsValid(GetAbilitySystemComponent()) && GameplayEffectClass)
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, EffectContextHandle);
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
	if(!HasAuthority()) return;

	//If there is authority, then we must grant the Ability, but its something that the ASC must do
	ASC->AddCharacterAbilities(StartupAbilities);
}
