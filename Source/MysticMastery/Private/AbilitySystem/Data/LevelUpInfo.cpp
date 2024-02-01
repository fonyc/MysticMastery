// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelUpInfo.h"

/** This method counts that the first element is a valid element, meaning Level = element + 1  */
int32 ULevelUpInfo::GetLevelByExperience(int32 XP) const 
{
	if (LevelUpInfo.IsEmpty()) return 1;
	for (int x = 0; x < LevelUpInfo.Num(); x++)
	{
		if (LevelUpInfo[x].LevelUpRequirement >= XP) return x + 1;
	}
	
	return 1;
}

int32 ULevelUpInfo::GetSpellPointsRewardByLevel(int32 InLevel) const
{
	const int32 SafeIndex = InLevel - 1 < 0 ? 0 : InLevel - 1; 
	return LevelUpInfo[SafeIndex].SpellPointAward;
}

int32 ULevelUpInfo::GetAttributePointsRewardByLevel(int32 InLevel) const
{
	const int32 SafeIndex = InLevel - 1 < 0 ? 0 : InLevel - 1; 
	return LevelUpInfo[SafeIndex].AttributePointAward;
}