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
	GetMMAbilitySystemComponent()->OnAbilitiesStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
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
	});
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const FMMGameplayTags GameplayTags = FMMGameplayTags::Get();
	const int32 CurrentSpellPoints = GetMMPlayerState()->GetSpellPoints();
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

	bool bEnableSpendButton = false;
	bool bEnableEquipButton = false;

	ShouldEnableButtons(AbilityStatus, CurrentSpellPoints, bEnableSpendButton, bEnableEquipButton);
	OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpendButton, bEnableEquipButton);
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
