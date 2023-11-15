// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Base.generated.h"

/**
 * 
 */

UCLASS()
class MYSTICMASTERY_API UMMC_Base : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_Base();

	//Function used to modify the attribute
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

	//Coefficient that will multiply each attribute
	UPROPERTY(EditAnywhere)
	TArray<float> Coefficients;

	UPROPERTY(EditAnywhere)
	float LevelMultiplier = 10.0f;

	//The base number of the formula 
	UPROPERTY(EditAnywhere)
	float PreMultiply = 1.0f;
};
