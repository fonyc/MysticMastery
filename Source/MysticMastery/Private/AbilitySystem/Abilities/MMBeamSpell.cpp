// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MMBeamSpell.h"
#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UMMBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UMMBeamSpell::StoreOwnerPlayerControllerAndCharacter()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UMMBeamSpell::TraceFirstTarget(const FVector BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			FHitResult HitResult;
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("Midsocket"));
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter,
				SocketLocation,
				BeamTargetLocation,
				10.f,
				TraceTypeQuery1,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				HitResult,
				true);

			if (HitResult.bBlockingHit)
			{
				MouseLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
	//Bind the actor mouse target to know when this actor died 
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UMMBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UMMBeamSpell::PrimaryTargetDied);
		}
	}
}

void UMMBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);

	TArray<AActor*> OverlappingActors;
	UMMAbilitySystemBlueprintLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		NextTargetRadius.GetValueAtLevel(GetAbilityLevel()),
		MouseHitActor->GetActorLocation());

	//const int32 NumAdditionalTargets = 5;
	const int32 NumAdditionalTargets = FMath::Min(MaxNumShockTargets, GetAbilityLevel() - 1);
	
	UMMAbilitySystemBlueprintLibrary::GetClosestTargets(NumAdditionalTargets, OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());

	//Bind additional targets to the on death delegate
	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UMMBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UMMBeamSpell::AdditionalTargetDied);
			}
		}
	}
}
