// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UMMAttributeSet* MMAttributeSet = CastChecked<UMMAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(MMAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(MMAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(MMAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(MMAttributeSet->GetMaxMana());
}

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

	if(UMMAbilitySystemComponent* MMASC = Cast<UMMAbilitySystemComponent>(AbilitySystemComponent))
	{
		if(MMASC->bStartupAbilitiesGiven) //The abilities has been given already, so we can perform the actions (such as broadcast the abilities to the widgets)
		{
			OnInitializeStartupAbilities(MMASC);
		}
		else //In this case, the abilities has not been given yet, so we bind them
		{
			//Abilities 
			MMASC->OnAbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		
			//Messages
			MMASC->EffectAssetTags.AddLambda(
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
	}
}

/**
 *
 * Look up for the AbilityInfo of every ability and broadcast it to the widgets so they have an icon, bg... etc
 * @param MMASC MMAbilitySystemComponent Holds the information about all the given abilities 
 */
void UOverlayWidgetController::OnInitializeStartupAbilities(UMMAbilitySystemComponent* MMASC)
{
	if (!MMASC->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, MMASC](const FGameplayAbilitySpec& AbilitySpec)
	{
		FMMAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(MMASC->GetAbilityTagBySpec(AbilitySpec));
		Info.InputTag = MMASC->GetInputTagBySpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	MMASC->ForEachAbility(BroadcastDelegate);
}
