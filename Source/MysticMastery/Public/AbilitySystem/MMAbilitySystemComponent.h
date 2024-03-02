// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MMAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FOnAbilitiesGiven);

//Delegate in charge of looping through every ability and give back the AbilityInfo (so we dont expose everything) 
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged,const FGameplayTag& /* Ability Tag*/, const FGameplayTag& /* Status Tag*/, int32 /*AbilityLevel*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquippedSignature,const FGameplayTag& /* Ability Tag*/, const FGameplayTag& /* Status Tag*/, const FGameplayTag& /*SlotTag*/, const FGameplayTag&/*PrevSlot*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbilitySignature, const FGameplayTag& /* Ability Tag */);

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

	//When leveling up, maybe one skill is eligible to buy 
	FAbilityStatusChanged OnAbilitiesStatusChanged;

	FAbilityEquippedSignature AbilityEquippedDelegate;
	FDeactivatePassiveAbilitySignature DeactivatePassiveAbilityDelegate;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	bool bStartupAbilitiesGiven = false;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate);
	
	static FGameplayTag GetAbilityTagBySpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagBySpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityStatusBySpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayTag GetSlotByAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetStatusByAbilityTag(const FGameplayTag& AbilityTag);
	bool SlotIsEmpty(const FGameplayTag& SlotTag);
	static bool AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& SlotTag);
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& Spec);
	FGameplayAbilitySpec* GetSpecBySlot(const FGameplayTag& SlotTag);
	bool IsPassiveAbility(const FGameplayAbilitySpec& Spec) const;
	static void AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& SlotTag);
	
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	
	void UpgradeAttributeByTag(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttributeByTag(const FGameplayTag& AttributeTag);

	//Update the ability statuses. When leveling up, maybe we reached the level requirement to but an ability and that
	void UpdateAbilityStatuses(int32 LevelRequirement);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag);

	UFUNCTION(Server, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& SlotTag, const FGameplayTag& PreviousSlot);

	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextDescription);

	static void ClearSlot(FGameplayAbilitySpec* Spec);

	void ClearAbilitiesFromSlot(const FGameplayTag& SlotTag); 
	static bool AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag SlotTag);
	
protected:
	/*
	 * Called on server whenever a GE is applied to self. This includes instant and duration based GEs
	 * We need to turn this function into an Client_RPC since it only executes on the server, but we want clients to be affected by them
	*/
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	/*
	 * Function called when we use Activate Abilities (when we give abilities to a player)
	 * In this case, we need it to replicate the Ability's icons on the client once we give a player its abilities
	 */
	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);
};
