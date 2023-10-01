// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MMPlayerState.h"

#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/MMAttributeSet.h"

AMMPlayerState::AMMPlayerState()
{
	//How often the server will update clients (ms)
	NetUpdateFrequency = 100.0f;

	//GAS
	AbilitySystemComponent = CreateDefaultSubobject<UMMAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UMMAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AMMPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
