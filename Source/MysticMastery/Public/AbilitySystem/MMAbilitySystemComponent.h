// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MMAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FOnAbilitiesGiven);

//Delegate in charge of looping through every ability and give back the AbilityInfo (so we dont expose everything) 
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);

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

	//Delegate called to broadcast the moment the abilities has been given (just a signal)
	FOnAbilitiesGiven OnAbilitiesGivenDelegate;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	bool bStartupAbilitiesGiven = false;

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate);
	
	static FGameplayTag GetAbilityTagBySpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagBySpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityStatusBySpec(const FGameplayAbilitySpec& AbilitySpec);
	
	void UpgradeAttributeByTag(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttributeByTag(const FGameplayTag& AttributeTag);

	
protected:
	// Called on server whenever a GE is applied to self. This includes instant and duration based GEs
	// We need to turn this function into an Client_RPC since it only executes on the server, but we want clients to be affected by them  
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	/**
	 * Function called when we use Activate Abilities (when we give abilities to a player)
	 * In this case, we need it to replicate the Ability's icons on the client once we give a player its abilities
	 */
	virtual void OnRep_ActivateAbilities() override;
};
