// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMGameplayAbility.h"
#include "MMProjectileSpell.generated.h"

class AMMProjectile;
/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMProjectileSpell : public UMMGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;


	UFUNCTION(BlueprintCallable, Category="Projectile")
	void SpawnProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMMProjectile> ProjectileClass;
};
