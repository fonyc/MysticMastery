// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/MMInputConfig.h"

const UInputAction* UMMInputConfig::FindAbilityInputActionByTag(const FGameplayTag& InputTag, bool bLogNotFound)
{
	for (const FMMInputAction& Action : AbilityInputActions)
	{
		if(Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}
	
	if (bLogNotFound) UE_LOG(LogTemp, Error, TEXT("Selected tag[%s] has no Input action related [%s]"),*InputTag.ToString(), *GetNameSafe(this));
	return nullptr;
}
