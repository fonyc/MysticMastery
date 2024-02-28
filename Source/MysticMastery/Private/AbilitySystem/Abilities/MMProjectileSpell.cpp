// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MMProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actors/MMProjectile.h"
#include "Interaction/CombatInterface.h"

void UMMProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMMProjectileSpell::SpawnProjectile(const FVector& ProjectileTarget, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{
	//Spawn projectile as replicated Actor -> Only if we are in the server
	if (const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); !bIsServer) return;

	FTransform SpawnTransform;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
		SpawnTransform.SetLocation(SocketLocation);

		FRotator Rotation = (ProjectileTarget - SocketLocation).Rotation();
		
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}
		
		SpawnTransform.SetRotation(Rotation.Quaternion());
	}

	//SpawnActorDeferred allows us to Spawn a class WITHOUT using its constructor, that will be something on the caller by calling UGameplayStatics::FinishSpawningActor
	AMMProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMMProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	//We dont know who is the target actor when the projectile spawns, that must be set on overlap
	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
	
	Projectile->FinishSpawning(SpawnTransform);
}
