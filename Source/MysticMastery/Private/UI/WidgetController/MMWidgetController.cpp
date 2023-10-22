// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/MMWidgetController.h"

void UMMWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
}

void UMMWidgetController::BroadcastInitialValues() {}

void UMMWidgetController::BindCallbacksToDependencies() {}
