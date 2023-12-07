// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MMProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actors/MMProjectile.h"
#include "Interaction/CombatInterface.h"

void UMMProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMMProjectileSpell::SpawnProjectile(const FVector& ProjectileTarget)
{
	//Spawn projectile as replicated Actor -> Only if we are in the server
	if (const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); !bIsServer) return;

	FTransform SpawnTransform;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		SpawnTransform.SetLocation(SocketLocation);

		FRotator Rotation = (ProjectileTarget - SocketLocation).Rotation();
		Rotation.Pitch = 0.f;
		SpawnTransform.SetRotation(Rotation.Quaternion());
	}

	//SpawnActorDeferred allows us to Spawn a class WITHOUT using its constructor, that will be something on the caller by calling UGameplayStatics::FinishSpawningActor
	AMMProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMMProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	const UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	const FGameplayEffectSpecHandle GameplayEffectSpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(),ASC->MakeEffectContext());
	Projectile->DamageEffectSpecHandle = GameplayEffectSpecHandle;
	Projectile->FinishSpawning(SpawnTransform);
}
