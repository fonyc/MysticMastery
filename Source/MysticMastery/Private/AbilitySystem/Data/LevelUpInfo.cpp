// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::GetLevelByExperience(int32 XP)
{
	if (LevelUpInfo.IsEmpty()) return 1;
	for (int x = 0; x < LevelUpInfo.Num(); x++)
	{
		if (LevelUpInfo[x].LevelUpRequirement >= XP) return x + 1;
	}
	
	return 1;
}
