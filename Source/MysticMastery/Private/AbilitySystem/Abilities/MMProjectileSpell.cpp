// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MMProjectileSpell.h"
#include "Kismet/KismetSystemLibrary.h"

void UMMProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UKismetSystemLibrary::PrintString(this, FString("Activate Ability in C++"),true,true,FLinearColor::Blue,5.f);
}
