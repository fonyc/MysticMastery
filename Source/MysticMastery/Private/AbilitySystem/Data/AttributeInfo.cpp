// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FMMAttributeInfo UAttributeInfo::FindAttributeInfoByTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (const FMMAttributeInfo& AttributeInfo : AttributeInformation)
	{
		if (AttributeInfo.AttributeTag.MatchesTagExact(Tag))
		{
			return AttributeInfo;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant find Info for Attribute Tag [%s] on AttributeInfo[%s]."), *Tag.ToString(),
		       *GetNameSafe(this));
	}
	return FMMAttributeInfo();
}
