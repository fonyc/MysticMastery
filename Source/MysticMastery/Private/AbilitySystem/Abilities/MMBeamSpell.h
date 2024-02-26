// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MMDamageGameplayAbility.h"
#include "MMBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMBeamSpell : public UMMDamageGameplayAbility
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void StoreOwnerPlayerControllerAndCharacter();
	
protected:
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;
};
