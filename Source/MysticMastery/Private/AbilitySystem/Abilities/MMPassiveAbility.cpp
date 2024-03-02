// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MMPassiveAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"

void UMMPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//Subscribe to be able to deactivate passive ability
	if (UMMAbilitySystemComponent* MMASC = Cast<UMMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		MMASC->DeactivatePassiveAbilityDelegate.AddUObject(this, &UMMPassiveAbility::ReceiveDeactivate);
	}
}

void UMMPassiveAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	if (AbilityTags.HasTagExact(AbilityTag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
