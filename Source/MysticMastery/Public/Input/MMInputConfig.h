// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MMInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FMMInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * 
 */

UCLASS()
class MYSTICMASTERY_API UMMInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:

	const UInputAction* FindAbilityInputActionByTag(const FGameplayTag& InputTag, bool bLogNotFound = false);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMMInputAction> AbilityInputActions;
};
