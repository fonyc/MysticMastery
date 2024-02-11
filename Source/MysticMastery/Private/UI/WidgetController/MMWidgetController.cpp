// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/MMWidgetController.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/MMPlayerController.h"
#include "Player/MMPlayerState.h"

void UMMWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
}

void UMMWidgetController::BroadcastInitialValues() {}

void UMMWidgetController::BindCallbacksToDependencies() {}

/**
 *
 * Look up for the AbilityInfo of every ability and broadcast it to the widgets so they have an icon, bg... etc
 * 
 */
void UMMWidgetController::BroadCastAbilityInfo()
{
	if (!GetMMAbilitySystemComponent()->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FMMAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(MMAbilitySystemComponent->GetAbilityTagBySpec(AbilitySpec));
		Info.InputTag = MMAbilitySystemComponent->GetInputTagBySpec(AbilitySpec);
		Info.StatusTag = MMAbilitySystemComponent->GetAbilityStatusBySpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	GetMMAbilitySystemComponent()->ForEachAbility(BroadcastDelegate);
}

AMMPlayerController* UMMWidgetController::GetMMPlayerController()
{
	if (MMPlayerController == nullptr)
	{
		MMPlayerController = Cast<AMMPlayerController>(PlayerController);
	}
	return MMPlayerController;
}

AMMPlayerState* UMMWidgetController::GetMMPlayerState()
{
	if (MMPlayerState == nullptr)
	{
		MMPlayerState = Cast<AMMPlayerState>(PlayerState);
	}
	return MMPlayerState;
}

UMMAbilitySystemComponent* UMMWidgetController::GetMMAbilitySystemComponent()
{
	if (MMAbilitySystemComponent == nullptr)
	{
		MMAbilitySystemComponent = Cast<UMMAbilitySystemComponent>(AbilitySystemComponent);
	}
	return MMAbilitySystemComponent;
}

UMMAttributeSet* UMMWidgetController::GetMMAttributeSet()
{
	if (MMAttributeSet == nullptr)
	{
		MMAttributeSet = Cast<UMMAttributeSet>(AttributeSet);
	}
	return MMAttributeSet;
}
