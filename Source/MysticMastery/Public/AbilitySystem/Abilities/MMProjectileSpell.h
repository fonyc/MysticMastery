// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMDamageGameplayAbility.h"
#include "MMProjectileSpell.generated.h"

struct FGameplayTag;
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
	void SpawnProjectile(const FVector& ProjectileTarget, const FGameplayTag& SocketTag, bool bOverridePitch = false, float PitchOverride = 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMMProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	int32 ProjectileNumber = 5;
};
