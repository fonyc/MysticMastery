// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/MMAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UMMAttributeSet* MMAttributeSet = CastChecked<UMMAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(MMAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(MMAttributeSet->GetMaxHealth());
}

//Function called once the Widget Controller is created (called for the 1st time) in the MMHUD GetOverlayWidgetController
void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UMMAttributeSet* MMAttributeSet = CastChecked<UMMAttributeSet>(AttributeSet);

	//Bind callbacks to the GAS system
	
	//Health
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAttributeSet->GetHealthAttribute()).AddUObject(
		this, &UOverlayWidgetController::HealthChanged);
	
	//MaxHealth
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAttributeSet->GetMaxHealthAttribute()).AddUObject(
		this, &UOverlayWidgetController::MaxHealthChanged);
}

//Functions to react when an Attribute is changed via GAS
void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}
