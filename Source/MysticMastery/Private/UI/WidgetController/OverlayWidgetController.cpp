// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "MMGameplayTags.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/MMPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetMMAttributeSet()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetMMAttributeSet()->GetMaxHealth());
	OnManaChanged.Broadcast(GetMMAttributeSet()->GetMana());
	OnMaxManaChanged.Broadcast(GetMMAttributeSet()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	//Bind Callbacks to the player State
	GetMMPlayerState()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	
	GetMMPlayerState()->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel)
	{
		OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
	});

	//Bind callbacks to the GAS system
	//Health
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMMAttributeSet()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnHealthChanged.Broadcast(Data.NewValue);});

	//MaxHealth
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMMAttributeSet()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnMaxHealthChanged.Broadcast(Data.NewValue);});

	//Mana
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMMAttributeSet()->GetManaAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data){OnManaChanged.Broadcast(Data.NewValue);});

	//MaxMana
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMMAttributeSet()->GetMaxManaAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data){OnMaxManaChanged.Broadcast(Data.NewValue);});

	if (GetMMAbilitySystemComponent())
	{
		GetMMAbilitySystemComponent()->AbilityEquippedDelegate.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		
		//The abilities has been given already, so we can perform the actions (such as broadcast the abilities to the widgets)
		if (GetMMAbilitySystemComponent()->bStartupAbilitiesGiven) 
		{
			BroadCastAbilityInfo();
		}
		else //In this case, the abilities has not been given yet, so we bind them
		{
			//Abilities 
			GetMMAbilitySystemComponent()->OnAbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadCastAbilityInfo);
			
			//Messages
			GetMMAbilitySystemComponent()->EffectAssetTags.AddLambda(
				[this](const FGameplayTagContainer& AssetTags)
				{
					for (const FGameplayTag& Tag : AssetTags)
					{
						if (!Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Message")))) continue;

						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				});
		}
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetMMPlayerState()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Level up info is missing inside the Overlay Widget Controller. Please fill it out on its Blueprint"));

	const int32 CurrentLevel = LevelUpInfo->GetLevelByExperience(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInfo.Num();

	if (CurrentLevel <= MaxLevel && CurrentLevel > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInfo[CurrentLevel - 1].LevelUpRequirement;

		int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInfo[FMath::Clamp(CurrentLevel - 2, 0, MaxLevel)].LevelUpRequirement;
		//In case we are at level 1, we need PreviousLevelUpRequirement not to be equal no the LevelUpRequirement
		if (PreviousLevelUpRequirement == LevelUpRequirement) PreviousLevelUpRequirement = 0;
			
		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) /  static_cast<float>(DeltaLevelRequirement);
		
		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
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
}
