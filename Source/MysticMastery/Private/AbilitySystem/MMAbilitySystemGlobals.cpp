// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MMAbilitySystemGlobals.h"

#include "MMAbilitySystemTypes.h"

FGameplayEffectContext* UMMAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FMMGameplayEffectContext();
}
