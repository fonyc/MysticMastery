// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MMProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actors/MMProjectile.h"
#include "Character/MMEnemy.h"
#include "Interaction/CombatInterface.h"
#include "MysticMastery/Public/MMGameplayTags.h"

void UMMProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMMProjectileSpell::SpawnProjectile(const FVector& ProjectileTarget, const FGameplayTag& SocketTag)
{
	//Spawn projectile as replicated Actor -> Only if we are in the server
	if (const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); !bIsServer) return;

	FTransform SpawnTransform;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
		SpawnTransform.SetLocation(SocketLocation);

		FRotator Rotation = (ProjectileTarget - SocketLocation).Rotation();
		//Rotation.Pitch = 0.f;
		SpawnTransform.SetRotation(Rotation.Quaternion());
	}

	AMMCharacterBase* CharacterBase = Cast<AMMCharacterBase>(GetAvatarActorFromActorInfo());
	TSubclassOf<AMMProjectile> ProjectileClassFromCharacter = CharacterBase->GetCharacterProjectileClass();

	TSubclassOf<AMMProjectile> SpawnProjectileClass = ProjectileClassFromCharacter == nullptr ? ProjectileClass : CharacterBase->GetCharacterProjectileClass();

	//SpawnActorDeferred allows us to Spawn a class WITHOUT using its constructor, that will be something on the caller by calling UGameplayStatics::FinishSpawningActor
	AMMProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMMProjectile>(
		SpawnProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());

	/** -- Create and Set FGameplayEffectContextHandle properties -- */
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	
	//SetAbility also sets: AbilityCDO, AbilityInstanceNotReplicated and AbilityLevel
	EffectContextHandle.SetAbility(this);
	
	EffectContextHandle.AddSourceObject(Projectile);

	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(Projectile);
	EffectContextHandle.AddActors(Actors);

	FHitResult HitResult;
	HitResult.Location = ProjectileTarget;
	EffectContextHandle.AddHitResult(HitResult);

	/** -- End -- */
	
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

	//Get GameplayTags to add the modifier magnitude in "Set by caller" (Damage tag in this case)
	FMMGameplayTags GameplayTags = FMMGameplayTags::Get();

	//Get Damage from the multiple damage tags and assign its value
	for(auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
	}

	Projectile->DamageEffectSpecHandle = SpecHandle;
	Projectile->FinishSpawning(SpawnTransform);
}
