// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MMAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/)

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	//Function called whenever a GE has its Ability Actor info Set
	void AbilityActorInfoSet();

	//Delegate called to broadcast the tags that affects a GE
	FEffectAssetTags EffectAssetTags;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);

protected:
	// Called on server whenever a GE is applied to self. This includes instant and duration based GEs
	// We need to turn this function into an Client_RPC since it only executes on the server, but we want clients to be affected by them  
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
};
