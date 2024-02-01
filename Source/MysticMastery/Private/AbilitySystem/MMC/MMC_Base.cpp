// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MMC/MMC_Base.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_Base::UMMC_Base()
{
}

float UMMC_Base::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	//Pass source and target tags to the aggregator
	FAggregatorEvaluateParameters EvaluationParameters;

	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//Get PlayerLevel (Variable required for the calculation)
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	float TotalAttributesValue = 0.f;
	int32 index = 0;
	
	for (auto Attribute : RelevantAttributesToCapture)
	{
		float OutValue = 0.f;
		float Coefficient = 1.0f;

		GetCapturedAttributeMagnitude(Attribute, Spec, EvaluationParameters, OutValue);
		OutValue = FMath::Max<float>(OutValue, 0.f);
		if(!Coefficients.IsEmpty() && Coefficients.IsValidIndex(index)) Coefficient = Coefficients[index];
		TotalAttributesValue += OutValue * Coefficient;
		index++;
	}

	//Perform the actual calculation (PreMultiply + SUM(Coeff1 * Att1 ... CoeffN * AttN) + LevelMultiplier * Level)
	return PreMultiply + TotalAttributesValue + LevelMultiplier * PlayerLevel;
}
