// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityInfo.h"
#include "MysticMastery/MMLogChannels.h"

FMMAbilityInfo UAbilityInfo::FindAbilityInfoByTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FMMAbilityInfo AbilityInfo : AbilityInformation)
	{
		if (AbilityInfo.AbilityTag == AbilityTag)
		{
			return AbilityInfo;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(MMLog, Error, TEXT("Can't find information for AbilityInfo [%s] with Tag: [%s]"), *GetNameSafe(this), *AbilityTag.ToString())
	}

	return FMMAbilityInfo();
}
