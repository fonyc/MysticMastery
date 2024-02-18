// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();
	void ApplyDebuffIfAble(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                       const FGameplayEffectSpec& Spec,
	                       const FAggregatorEvaluateParameters& EvaluationParameters,
	                       const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>&) const;

	//What type of calculations are going to execute ?
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
