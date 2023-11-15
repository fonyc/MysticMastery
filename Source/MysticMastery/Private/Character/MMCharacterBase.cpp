// Fill out your copyright notice in the Description page of Project Settings.


#include "MysticMastery/Public/Character/MMCharacterBase.h"
#include "AbilitySystemComponent.h"

// Sets default values
AMMCharacterBase::AMMCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));

	//Remove any collision from weapon
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AMMCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
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
