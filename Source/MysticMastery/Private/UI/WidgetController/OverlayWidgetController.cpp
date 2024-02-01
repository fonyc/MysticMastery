// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/MMPlayerState.h"

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
	//Bind Callbacks to the player State
	AMMPlayerState* MMPlayerState = CastChecked<AMMPlayerState>(PlayerState);
	MMPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	
	MMPlayerState->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel)
	{
		OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
	});

	//Bind callbacks to the GAS system
	const UMMAttributeSet* MMAttributeSet = CastChecked<UMMAttributeSet>(AttributeSet);
	
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
				});
		}
	}
}

/**
 *
 * Look up for the AbilityInfo of every ability and broadcast it to the widgets so they have an icon, bg... etc
 * @param MMASC MMAbilitySystemComponent Holds the information about all the given abilities
 * 
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

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const AMMPlayerState* MMPlayerState = CastChecked<AMMPlayerState>(PlayerState);
	const ULevelUpInfo* LevelUpInfo = MMPlayerState->LevelUpInfo;

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
