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

	/**
	 * Returns the Attribute points awarded when leveling up. It looks inside the DA_LevelUpInfo 
	 */
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(const int32 InLevel) const;

	/**
	 * Returns the Spell points awarded when leveling up. It looks inside the DA_LevelUpInfo 
	 */
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(const int32 InLevel) const;

	/**
	 * Return the Attribute points AVAILABLE to spend. It looks inside the Player State 
	 */
	UFUNCTION(BlueprintNativeEvent)
	int32 GetCurrentAttributePoints() const;

	/**
	 * Return the Spell points AVAILABLE to spend. It looks inside the Player State 
	 */
	UFUNCTION(BlueprintNativeEvent)
	int32 GetCurrentSpellPoints() const;
	
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
