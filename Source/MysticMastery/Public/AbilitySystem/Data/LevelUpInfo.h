// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT()
struct FMMLevelUpInfo
{
	GENERATED_BODY()

	//Amount of EXP required to level up
	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;

	//When Leveling up, the amount of attribute points to spend
	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1;

	//When Leveling up, the amount of spell points to spend
	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointAward = 1;
};

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FMMLevelUpInfo> LevelUpInfo;

	int32 GetLevelByExperience(int32 XP) const;

	int32 GetSpellPointsRewardByLevel(int32 InLevel) const;

	int32 GetAttributePointsRewardByLevel(int32 InLevel) const;
};
