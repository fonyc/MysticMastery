// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MMProjectileSpell.h"
#include "MMFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMFireBolt : public UMMProjectileSpell
{
	GENERATED_BODY()
	
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnMultipleProjectiles(const FVector& ProjectileTarget, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget);

protected:
	
	UPROPERTY(EditDefaultsOnly, Category= "FireBolt")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category= "FireBolt")
	int32 MaxNumProjectiles = 5;
	
};
