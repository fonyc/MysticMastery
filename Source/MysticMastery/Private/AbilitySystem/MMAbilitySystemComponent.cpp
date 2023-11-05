// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/MMAbilitySystemComponent.h"

void UMMAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UMMAbilitySystemComponent::EffectApplied);
}

//Whenever we apply a GE, this method will be called
void UMMAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                              const FGameplayEffectSpec& EffectSpec,
                                              FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}
