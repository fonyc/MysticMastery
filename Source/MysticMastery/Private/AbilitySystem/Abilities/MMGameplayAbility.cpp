// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MMGameplayAbility.h"

FString UMMGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name - Lorem ipsum dolor sit amet, mco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nu qui officia deserunt mollit anim id est laborum", Level);
}

FString UMMGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default> Causes much more damage bla bla </>"),Level);
}

FString UMMGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Ability Locked until Level: </><Level>%d</>"),Level);
}
