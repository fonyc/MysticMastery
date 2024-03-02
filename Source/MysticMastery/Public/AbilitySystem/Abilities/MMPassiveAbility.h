// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMGameplayAbility.h"
#include "MMPassiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMPassiveAbility : public UMMGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	void ReceiveDeactivate(const FGameplayTag& AbilityTag);
};
