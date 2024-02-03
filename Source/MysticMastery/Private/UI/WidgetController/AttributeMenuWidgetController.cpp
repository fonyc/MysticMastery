// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/MMPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UMMAttributeSet* AS = CastChecked<UMMAttributeSet>(AttributeSet);
	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	//Bind Callbacks to the player State (Attribute Points)
	AMMPlayerState* MMPlayerState = CastChecked<AMMPlayerState>(PlayerState);
	MMPlayerState->OnAttributePointsChanged.AddLambda([this](int32 Points)
		{
			AttributePointsChangedDelegate.Broadcast(Points);
		}
	);
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UMMAttributeSet* AS = CastChecked<UMMAttributeSet>(AttributeSet);

	check(AttributeInfo);

	//Iterate all key-value tags and Broadcast Attributes initial values
	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	//Broadcast initial values to the attribute controller
	AMMPlayerState* MMPlayerState = CastChecked<AMMPlayerState>(PlayerState);
	AttributePointsChangedDelegate.Broadcast(MMPlayerState->GetAttributePoints());
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,const FGameplayAttribute& Attribute) const
{
	FMMAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
