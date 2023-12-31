// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMDamageGameplayAbility.h"
#include "MMProjectileSpell.generated.h"

class AMMProjectile;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMProjectileSpell : public UMMDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;


	UFUNCTION(BlueprintCallable, Category="Projectile")
	void SpawnProjectile(const FVector& ProjectileTarget);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMMProjectile> ProjectileClass;

};
