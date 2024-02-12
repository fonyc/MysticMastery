// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OverlayWidgetController.h"
#include "UI/WidgetController/MMWidgetController.h"
#include "GameplayTagContainer.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MYSTICMASTERY_API USpellMenuWidgetController : public UMMWidgetController
{
	GENERATED_BODY()
public:
	
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature OnSpellPointsChanged;

	//Used to broadcast when an ability is selected, if it enable or not the equip/spendPoint buttons
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature OnSpellGlobeSelectedDelegate;
	
	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

private:
	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpendButton, bool& bShouldEnableEquipButton);
};
