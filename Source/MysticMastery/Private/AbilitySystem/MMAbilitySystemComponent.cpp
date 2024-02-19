// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/MMAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MMGameplayTags.h"
#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Abilities/MMGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Interaction/PlayerInterface.h"
#include "MysticMastery/MMLogChannels.h"

void UMMAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UMMAbilitySystemComponent::ClientEffectApplied);
}

void UMMAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const auto AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		if (const UMMGameplayAbility* MMAbility = Cast<UMMGameplayAbility>(AbilitySpec.Ability))
		{
			//add the StartupInputTag to the ability 
			AbilitySpec.DynamicAbilityTags.AddTag(MMAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FMMGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	OnAbilitiesGivenDelegate.Broadcast();
}

void UMMAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const auto AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UMMAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UMMAbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UMMAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	//Lock the abilities while we loop through them. Abilities can change with a gameplay tag so we prevent that
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(MMLog, Error, TEXT("Failed to execute delegate in [%hs]"), __FUNCTION__);
		}
	}
}

FGameplayTag UMMAbilitySystemComponent::GetAbilityTagBySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	UE_LOG(MMLog, Error, TEXT("Requested Ability for AbilitySpec is null or there are no tags inside"));
	return FGameplayTag();
}

FGameplayTag UMMAbilitySystemComponent::GetInputTagBySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	UE_LOG(MMLog, Error, TEXT("The AbilitySpec selected has no matches with type <InputTag> GameplayTag"));
	return FGameplayTag();
}

//Checks if the Ability has the tag of type "Abilities.Status". If so, return it. Else return an empty one
FGameplayTag UMMAbilitySystemComponent::GetAbilityStatusBySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UMMAbilitySystemComponent::GetInputTagByAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagBySpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayTag UMMAbilitySystemComponent::GetStatusByAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetAbilityStatusBySpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UMMAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag)) return &AbilitySpec;
		}
	}
	return nullptr;
}

void UMMAbilitySystemComponent::UpgradeAttributeByTag(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetCurrentAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttributeByTag(AttributeTag);
		}
	}
}

void UMMAbilitySystemComponent::UpdateAbilityStatuses(int32 LevelRequirement)
{
	UAbilityInfo* AbilityInfo = UMMAbilitySystemBlueprintLibrary::GetAbilityInfo(GetAvatarActor());

	for (const FMMAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		//Ability tag not valid or The level up didnt reach the ability requirements --> we don bother on update that one
		if (LevelRequirement < Info.LevelRequirement || !Info.AbilityTag.IsValid()) continue;
		//This ability is not on our activatable abilities --> must be locked 
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			//TODO: The level of the ability is 1 unless we are loading the ability from disk. 
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(FMMGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);
			ClientUpdateAbilityStatus(Info.AbilityTag, FMMGameplayTags::Get().Abilities_Status_Eligible, 1);
		}
	}
}

void UMMAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag)
{
	//If there is no AbilitySpec means the ability is not inside the activatable array --> we dont have it
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FMMGameplayTags& GameplayTags = FMMGameplayTags::Get();
		const FGameplayTag& PreviousSlot = GetInputTagBySpec(*AbilitySpec);
		const FGameplayTag& Status = GetAbilityStatusBySpec(*AbilitySpec);

		//Check that the ability can be equipped (we cannot equip locked or eligible)
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked) || Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
		{
			//Remove this Slot InputTag from anu ability that has it
			ClearAbilitiesFromSlot(SlotTag);
			//Clear this ability slot just in case is a different slot
			ClearSlot(AbilitySpec);
			//Assign the ability to the slot
			AbilitySpec->DynamicAbilityTags.AddTag(SlotTag);
			if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
			{
				AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Unlocked);
				AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
			}
			MarkAbilitySpecDirty(*AbilitySpec);
		}
		ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, SlotTag, PreviousSlot);
	}
}

void UMMAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& SlotTag, const FGameplayTag& PreviousSlot)
{
	AbilityEquippedDelegate.Broadcast(AbilityTag, Status, SlotTag, PreviousSlot);
}

bool UMMAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextDescription)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UMMGameplayAbility* MMAbility = Cast<UMMGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = MMAbility->GetDescription(AbilitySpec->Level);
			OutNextDescription = MMAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}

	//Ability is not inside activatable abilities -> is locked (no description)
	const UAbilityInfo* AbilityInfo = UMMAbilitySystemBlueprintLibrary::GetAbilityInfo(GetAvatarActor());
	if (AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FMMGameplayTags::Get().Abilities_None))
	{
		OutDescription = FString();
	}
	else
	{
		OutDescription = UMMGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoByTag(AbilityTag).LevelRequirement);
	}
	OutNextDescription = FString();
	return false;
}

void UMMAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag& SlotTag = GetInputTagBySpec(*Spec);
	Spec->DynamicAbilityTags.RemoveTag(SlotTag);
	MarkAbilitySpecDirty(*Spec);
}

void UMMAbilitySystemComponent::ClearAbilitiesFromSlot(const FGameplayTag& SlotTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(&Spec, SlotTag))
		{
			ClearSlot(&Spec);
		}
	}
}

bool UMMAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag SlotTag)
{
	for (FGameplayTag Tag : Spec->DynamicAbilityTags)
	{
		if (Tag.MatchesTagExact(SlotTag))
		{
			return true;
		}
	}
	return false;
}

void UMMAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	//If we dont have Ability Spec it means it is not inside the ActivatableAbilities (so it must be locked)
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		//Spend the SP
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddSpellPoints(GetAvatarActor(), -1);
		}
		//Make the change on the tags and level up the ability
		const FMMGameplayTags GameplayTags = FMMGameplayTags::Get();
		FGameplayTag Status = GetAbilityStatusBySpec(*AbilitySpec);
		//The ability is Eligible (1st time a point is spent on it)--> change of tag (Default level is 1)
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
			Status = GameplayTags.Abilities_Status_Unlocked;
		}
		//The ability is Equipped or Unlocked (at least 1 point invested on the ability) --> We level up the ability
		else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec->Level++;
		}
		//Broadcast the ability to the widgets
		ClientUpdateAbilityStatus_Implementation(AbilityTag, Status, AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

/**
 * Server Upgrade uses the GAS Event listener that every player has to upgrade the attribute
 */
void UMMAbilitySystemComponent::ServerUpgradeAttributeByTag_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddAttributePoints(GetAvatarActor(), -1);
	}
}

void UMMAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		OnAbilitiesGivenDelegate.Broadcast();
	}
}

void UMMAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	OnAbilitiesStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

//Whenever we apply a GE, this method will be called
void UMMAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                   const FGameplayEffectSpec& EffectSpec,
                                                                   FActiveGameplayEffectHandle
                                                                   ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}
