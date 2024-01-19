// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMGameplayAbility.h"
#include "MMSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMSummonAbility : public UMMGameplayAbility
{
	GENERATED_BODY()

public:
	/**
	 * 
	 * @return Allowed locations in which the Pawn minion can be spawned
	 */
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	UFUNCTION(BlueprintPure)
	FORCEINLINE TSubclassOf<APawn> GetRandomMinionClass();

	/** Max number of minions that can be spawned at the same time */
	UPROPERTY(EditDefaultsOnly, Category= "Summon Settings")
	int32 NumMinions = 5;
	
	UPROPERTY(EditDefaultsOnly, Category="Summon Settings")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category="Summon Settings")
	float MinSpawnDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Summon Settings")
	float MaxSpawnDistance = 250.f;

	/** Angle of spawn, so the invocations are not straight in front the summoner */
	UPROPERTY(EditDefaultsOnly)
	float SpawnSpread = 90.f;
};
