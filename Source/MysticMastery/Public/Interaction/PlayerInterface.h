// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYSTICMASTERY_API IPlayerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(const int32 InLevel) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(const int32 InLevel) const;
	
	UFUNCTION(BlueprintNativeEvent)
	void AddXP(int32 DeltaXP);

	UFUNCTION(BlueprintNativeEvent)
	void AddSpellPoints(int32 InSpellPoints);

	UFUNCTION(BlueprintNativeEvent)
	void AddAttributePoints(int32 InAttributePoints);

	UFUNCTION(BlueprintNativeEvent)
	void AddPlayerLevel(int32 InLevels);

	/* This functions resides in the PlayerState, but its here in the interface so we dont make a circular dependency between Attribute Set and Character */
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelByXP(int32 XP);
	
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
};
