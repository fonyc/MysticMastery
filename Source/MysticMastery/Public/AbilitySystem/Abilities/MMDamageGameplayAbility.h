// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMAbilitySystemTypes.h"
#include "AbilitySystem/Abilities/MMGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "MMDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMDamageGameplayAbility : public UMMGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag DamageType;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	FScalableFloat DebuffChance;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	FScalableFloat DebuffDamage;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float DebuffDuration = 5.f;
};
