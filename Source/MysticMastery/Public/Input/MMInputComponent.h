// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "MMInputConfig.h"
#include "MMInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	//Template used to Bind, using the InputConfig data asset to bind for a given key-Tag pair a callback 
	template <class UserClass, typename PressedFunctionType, typename ReleasedFunctionType, typename HeldFunctionType>
	void BindAbilityActions(const UMMInputConfig* InputConfig, UserClass* Object, PressedFunctionType PressedFunc, ReleasedFunctionType ReleasedFunc, HeldFunctionType HeldFunc);
};

template <class UserClass, typename PressedFunctionType, typename ReleasedFunctionType, typename HeldFunctionType>
void UMMInputComponent::BindAbilityActions(const UMMInputConfig* InputConfig, UserClass* Object, PressedFunctionType PressedFunc, ReleasedFunctionType ReleasedFunc, HeldFunctionType HeldFunc)
{
	check(InputConfig);

	for (const FMMInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}

			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}
}
