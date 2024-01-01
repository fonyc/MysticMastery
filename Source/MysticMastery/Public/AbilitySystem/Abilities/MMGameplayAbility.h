// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MMGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	//If the ability is set as initial Ability, then this will be its default input key
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;
};
