// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/MMAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UMMAttributeSet* MMAttributeSet = CastChecked<UMMAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(MMAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(MMAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(MMAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(MMAttributeSet->GetMaxMana());
}

//Function called once the Widget Controller is created (called for the 1st time) in the MMHUD GetOverlayWidgetController
void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UMMAttributeSet* MMAttributeSet = CastChecked<UMMAttributeSet>(AttributeSet);

	//Bind callbacks to the GAS system

	//Health
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnHealthChanged.Broadcast(Data.NewValue);});

	//MaxHealth
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnMaxHealthChanged.Broadcast(Data.NewValue);});

	//Mana
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAttributeSet->GetManaAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data){OnManaChanged.Broadcast(Data.NewValue);});

	//MaxMana
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAttributeSet->GetMaxManaAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data){OnMaxManaChanged.Broadcast(Data.NewValue);});

	Cast<UMMAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				if (!Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Message")))) continue;

				const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
				MessageWidgetRowDelegate.Broadcast(*Row);
			}
		}
	);
}