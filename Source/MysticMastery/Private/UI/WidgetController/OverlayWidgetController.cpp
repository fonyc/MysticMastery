// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

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
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAttributeSet->GetHealthAttribute()).AddUObject(
		this, &UOverlayWidgetController::HealthChanged);
	
	//MaxHealth
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAttributeSet->GetMaxHealthAttribute()).AddUObject(
		this, &UOverlayWidgetController::MaxHealthChanged);

	//Mana
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAttributeSet->GetManaAttribute()).AddUObject(
		this, &UOverlayWidgetController::ManaChanged);

	//MaxMana
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAttributeSet->GetMaxManaAttribute()).AddUObject(
		this, &UOverlayWidgetController::MaxManaChanged);
}

//Functions to react when an Attribute is changed via GAS

#pragma region ATTRIBUTE METHOD CALLBACKS
void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
#pragma endregion 