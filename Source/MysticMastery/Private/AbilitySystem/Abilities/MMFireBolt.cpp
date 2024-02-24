// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MMFireBolt.h"

#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"
#include "Actors/MMProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

FString UMMFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = -GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const float BurnChance = DebuffDamage.GetValueAtLevel(Level);
	const float BurnDamage = DebuffDamage.GetValueAtLevel(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
		//Title
		"<Title>FIRE BOLT</>\n\n"

		//Spell Details (Level + ManaCost + Cooldown)
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		//Spell Description (Damage)
		"<Default>Launches a bolt of fire, exploding on impact and dealing </>"
		"<Damage>%d</><Default> fire damage  with a </><Damage>%.1f%%</><Default> chance to deal </><Damage>%.1f</><Default> burn damage for 5 seconds.</>\n\n"),
		Level, ManaCost, Cooldown, ScaledDamage, BurnChance, BurnDamage);
	}
	return FString::Printf(TEXT(
	//Title
	"<Title>FIRE BOLT</>\n\n"

	//Spell Details (Level + ManaCost + Cooldown)
	"<Small>Level: </><Level>%d</>\n"
	"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
	"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

	//Spell Description (ProjectileNumber + Damage)
	"<Default>Launches %d bolts of fire, exploding on impact and dealing </>"
	"<Damage>%d</><Default> fire damage  with a </><Damage>%.1f%%</><Default> chance to deal </><Damage>%.1f</><Default> burn damage for 5 seconds.</>\n\n"),
		
	Level, ManaCost, Cooldown, FMath::Min(Level, ProjectileNumber), ScaledDamage, BurnChance, BurnDamage * 5.f);
}

FString UMMFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level + 1);
	const float ManaCost = -GetManaCost(Level + 1);
	const float Cooldown = GetCooldown(Level + 1);
	
	const float BurnChance = DebuffDamage.GetValueAtLevel(Level + 1);
	const float BurnDamage = DebuffDamage.GetValueAtLevel(Level + 1);
	return FString::Printf(TEXT(
	//Title
	"<Title>NEXT LEVEL</>\n\n"

	//Spell Details (Level + ManaCost + Cooldown)
	"<Small>Level: </><Level>%d</>\n"
	"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
	"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

	//Spell Description (ProjectileNumber + Damage)
	"<Default>Launches %d bolts of fire, exploding on impact and dealing </>"
	"<Damage>%d</><Default> fire damage  with a </><Damage>%.1f%%</><Default> chance to deal </><Damage>%.1f</><Default> burn damage for 5 seconds.</>\n\n"),
	
	Level, ManaCost, Cooldown, FMath::Min(Level, ProjectileNumber), ScaledDamage, BurnChance, BurnDamage * 5.f);
}

void UMMFireBolt::SpawnMultipleProjectiles(const FVector& ProjectileTarget, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	//Spawn projectile as replicated Actor -> Only if we are in the server
	if (const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); !bIsServer) return;
	
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);

		FRotator Rotation = (ProjectileTarget - SocketLocation).Rotation();
		
		if (bOverridePitch) Rotation.Pitch = PitchOverride;
		
		const FVector Forward = Rotation.Vector();
		const int32 ProjectilesToSpawn = FMath::Min(ProjectileNumber, GetAbilityLevel());
		TArray<FRotator> Rotations = UMMAbilitySystemBlueprintLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, SpreadPerProjectile * ProjectilesToSpawn, ProjectilesToSpawn);

		for (const FRotator& ProjectileRotation : Rotations)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SocketLocation);
			SpawnTransform.SetRotation(ProjectileRotation.Quaternion());
			
			/** -- Start Spawn Projectile */
			AMMProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMMProjectile>(
				ProjectileClass,
				SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

			//We are not clicking a wall / floor
			if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
			{
				Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
			}
			else
			{
				Projectile->HomingSceneReference = NewObject<USceneComponent>(USceneComponent::StaticClass());
				Projectile->HomingSceneReference->SetWorldLocation(ProjectileTarget);
				Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingSceneReference;
			}

			Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(MinHomingAcceleration, MaxHomingAcceleration);
			Projectile->ProjectileMovement->bIsHomingProjectile = bIsHomingProjectile;
			
			Projectile->FinishSpawning(SpawnTransform);
			/** -- End Spawn Projectile */
		}
	}
}
