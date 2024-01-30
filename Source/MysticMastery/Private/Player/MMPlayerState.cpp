// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MMPlayerState.h"

#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "Net/UnrealNetwork.h"

AMMPlayerState::AMMPlayerState()
{
	//How often the server will update clients (ms)
	NetUpdateFrequency = 100.0f;

	//GAS
	AbilitySystemComponent = CreateDefaultSubobject<UMMAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UMMAttributeSet>("AttributeSet");
}

void AMMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMMPlayerState, Level);
	DOREPLIFETIME(AMMPlayerState, XP);
}

UAbilitySystemComponent* AMMPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMMPlayerState::AddXP(int DeltaXP)
{
	XP += DeltaXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AMMPlayerState::SetXP(int NewXP)
{
	XP = NewXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AMMPlayerState::AddLevel(int DeltaLevel)
{
	Level += DeltaLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AMMPlayerState::SetLevel(int NewLevel)
{
	Level = NewLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AMMPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level);
}

void AMMPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}
