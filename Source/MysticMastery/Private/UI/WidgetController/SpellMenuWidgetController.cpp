// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "MMGameplayTags.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/MMPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadCastAbilityInfo();
	OnSpellPointsChanged.Broadcast(GetMMPlayerState()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetMMAbilitySystemComponent()->OnAbilitiesStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
	{
		if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.Status = StatusTag;
			
			bool bEnableSpendButton = false;
			bool bEnableEquipButton = false;
			ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendButton, bEnableEquipButton);

			FString Description;
			FString NextLevelDescription;
			GetMMAbilitySystemComponent()->GetDescriptionsByAbilityTag(AbilityTag,Description,NextLevelDescription);

			OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpendButton, bEnableEquipButton, Description, NextLevelDescription);
		}
		
		if (AbilityInfo)
		{
			FMMAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetMMPlayerState()->OnSpellPointsChanged.AddLambda([this](int32 SpellPoints)
	{
		OnSpellPointsChanged.Broadcast(SpellPoints);
		CurrentSpellPoints = SpellPoints;

		bool bEnableSpendButton = false;
		bool bEnableEquipButton = false;
		ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendButton, bEnableEquipButton);
		
		FString Description;
		FString NextLevelDescription;
		GetMMAbilitySystemComponent()->GetDescriptionsByAbilityTag(SelectedAbility.Ability,Description,NextLevelDescription);

		OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpendButton, bEnableEquipButton, Description, NextLevelDescription);
	});

	GetMMAbilitySystemComponent()->AbilityEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
		OnStopWaitingForEquipSelectionDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}
	
	const FMMGameplayTags GameplayTags = FMMGameplayTags::Get();
	const int32 SpellPoints = GetMMPlayerState()->GetSpellPoints();
	FGameplayTag AbilityStatus;
	
	const bool bTagIsValid = AbilityTag.IsValid();
	const bool bTagIsNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	const FGameplayAbilitySpec* AbilitySpec = GetMMAbilitySystemComponent()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecIsValid = AbilitySpec != nullptr;
	//An ability tag for an ability that we dont have yet or that doesnt exist
	if (!bTagIsValid || bTagIsNone || !bSpecIsValid)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetMMAbilitySystemComponent()->GetAbilityStatusBySpec(*AbilitySpec);
	}

	SelectedAbility.Status = AbilityStatus;
	SelectedAbility.Ability = AbilityTag;
	
	bool bEnableSpendButton = false;
	bool bEnableEquipButton = false;
	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendButton, bEnableEquipButton);
	
	FString Description;
	FString NextLevelDescription;
	GetMMAbilitySystemComponent()->GetDescriptionsByAbilityTag(AbilityTag,Description,NextLevelDescription);

	OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpendButton, bEnableEquipButton, Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	//Talk to ASC to upgrade a skill and remove 1SP from Playerstate
	if(GetMMAbilitySystemComponent())
	{
		GetMMAbilitySystemComponent()->ServerSpendSpellPoint(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::RemoveSelection()
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
		OnStopWaitingForEquipSelectionDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}
	
	SelectedAbility.Ability = FMMGameplayTags::Get().Abilities_None;
	SelectedAbility.Status = FMMGameplayTags::Get().Abilities_Status_Locked;

	OnSpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

//Tell the widget on the spell menu that needs to call the animation on the equip side
void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;

	OnWaitForEquipSelectionDelegate.Broadcast(AbilityType);
	bWaitingForEquipSelection = true;

	const FGameplayTag SelectedStatus = GetMMAbilitySystemComponent()->GetStatusByAbilityTag(SelectedAbility.Ability);

	if (SelectedStatus.MatchesTagExact(FMMGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = GetMMAbilitySystemComponent()->GetInputTagByAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellRowButtonPressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection) return;
	//Check selected ability against slot ability type (dont equip an offensive spell in a passive slot and vice versa)
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	GetMMAbilitySystemComponent()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	bWaitingForEquipSelection = false;
	const FMMGameplayTags& GameplayTags = FMMGameplayTags::Get();
	
	FMMAbilityInfo LastSlotAbilityInfo;
	LastSlotAbilityInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotAbilityInfo.InputTag = PreviousSlot;
	LastSlotAbilityInfo.AbilityTag = GameplayTags.Abilities_None;

	//Broadcast empty info if prevSlot is a valid slot (only equipping an already equipped spell)
	AbilityInfoDelegate.Broadcast(LastSlotAbilityInfo);

	FMMAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);

	OnStopWaitingForEquipSelectionDelegate.Broadcast(AbilityInfo->FindAbilityInfoByTag(AbilityTag).AbilityType);
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	RemoveSelection();
}


void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpendButton, bool& bShouldEnableEquipButton)
{
	const FMMGameplayTags GameplayTags = FMMGameplayTags::Get();

	bShouldEnableEquipButton = false;
	bShouldEnableSpendButton = false;
	
	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton = true;
		bShouldEnableSpendButton = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		bShouldEnableEquipButton = false;
		bShouldEnableSpendButton = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
		bShouldEnableSpendButton = SpellPoints > 0;
	}
}
