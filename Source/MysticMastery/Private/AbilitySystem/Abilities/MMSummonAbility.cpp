#include "AbilitySystem/Abilities/MMSummonAbility.h"

TArray<FVector> UMMSummonAbility::GetSpawnLocations()
{
	const FVector SummonerForward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector SummonerLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

	const FVector LeftSpread = SummonerForward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);

	const float DeltaSpread = SpawnSpread / NumMinions;
	TArray<FVector> SpawnLocations;
	for (int32 x = 0; x < NumMinions; ++x)
	{
		const FVector SummonDirection = LeftSpread.RotateAngleAxis(DeltaSpread * x, FVector::UpVector);
		FVector ChosenSpawnLocation = SummonerLocation + SummonDirection * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		//Trace a Hit Up and Down the Spawn point to check the boundaries and terrain
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f,0.f,400.f),ChosenSpawnLocation - FVector(0.f,0.f,400.f), ECC_Visibility);

		if (Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
		}
		
		SpawnLocations.Add(ChosenSpawnLocation);
	}
	return SpawnLocations;
}

TSubclassOf<APawn> UMMSummonAbility::GetRandomMinionClass()
{
	return MinionClasses[FMath::RandRange(0,MinionClasses.Num() -1)];
}
