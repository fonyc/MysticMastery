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

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector BeamTargetLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetRadiusFromCurveTable() { return NextTargetRadius.GetValueAtLevel(GetAbilityLevel()); }
	
protected:
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly)
	FScalableFloat NextTargetRadius;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxNumShockTargets = 5;

	//When a beam hit an actor, store nearby possible new targets
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);
};
