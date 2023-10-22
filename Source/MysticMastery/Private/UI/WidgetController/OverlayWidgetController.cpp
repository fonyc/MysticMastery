// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/MMAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UMMAttributeSet* MMAttributeSet = CastChecked<UMMAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(MMAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(MMAttributeSet->GetMaxHealth());
}
