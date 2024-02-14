// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MMProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actors/MMProjectile.h"
#include "Interaction/CombatInterface.h"
#include "MysticMastery/Public/MMGameplayTags.h"

FString UMMProjectileSpell::GetDescription(int32 Level)
{
	const int32 Damage = DamageTypes[FMMGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n<Default>Launches a bolt of fire, exploding on impact and dealing: </><Damage>%d</><Default> fire damage with a chance to burn.</>\n\n<Small>Level: </><Level>%d</>"), Damage, Level);
	}
	return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n<Default>Launches %d bolts of fire, exploding on impact and dealing: </><Damage>%d</><Default> fire damage with a chance to burn.</>\n\n<Small>Level: </><Level>%d</>"), FMath::Min(Level, ProjectileNumber), Damage, Level);
}

FString UMMProjectileSpell::GetNextLevelDescription(int32 Level)
{
	const int32 Damage = DamageTypes[FMMGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);
	return FString::Printf(TEXT("<Title>NEXT LEVEL: </>\n\n<Default>Launches %d bolts of fire, exploding on impact and dealing: </><Damage>%d</><Default> fire damage with a chance to burn.</>\n\n<Small>Level: </><Level>%d</>"), FMath::Min(Level, ProjectileNumber), Damage, Level);
}

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

	//SpawnActorDeferred allows us to Spawn a class WITHOUT using its constructor, that will be something on the caller by calling UGameplayStatics::FinishSpawningActor
	AMMProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMMProjectile>(
		ProjectileClass,
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
