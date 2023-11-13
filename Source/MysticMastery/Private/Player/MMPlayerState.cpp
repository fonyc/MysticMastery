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
}

UAbilitySystemComponent* AMMPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMMPlayerState::OnRep_Level(int32 OldLevel)
{
	
}
