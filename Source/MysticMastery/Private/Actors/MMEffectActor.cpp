// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MMEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"

AMMEffectActor::AMMEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}


// Called when the game starts or when spawned
void AMMEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMMEffectActor::ApplyGameplayEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	const bool bIsEnemy = TargetActor->ActorHasTag(FName("Enemy"));
	if(bIsEnemy && !bApplyEffectOnEnemies) return;
	
	//This is the same as getting the interface from an actor and cast it, but shorter and fancier
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	check(GameplayEffectClass);

	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle GameplayEffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpecHandle.Data.Get());

	//Add Infinite GE to a list so we can remove it if needed (Instant and Duration GE are removed on its own)
	const bool bIsInfinite = GameplayEffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetASC);
	}

	//Destroy Actor if the GE is instant and actor destruction is enabled
	if (!bIsInfinite)
	{
		Destroy();
	} 
}

void AMMEffectActor::OnBeginOverlap(AActor* TargetActor)
{
	const bool bIsEnemy = TargetActor->ActorHasTag(FName("Enemy"));
	if(bIsEnemy && !bApplyEffectOnEnemies) return;
	
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const auto& InstantEffect : InstantGameplayEffects)
		{
			ApplyGameplayEffectToTarget(TargetActor, InstantEffect);
		}
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const auto& DurationEffect : DurationGameplayEffects)
		{
			ApplyGameplayEffectToTarget(TargetActor, DurationEffect);
		}
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const auto& InfiniteEffect : InfiniteGameplayEffects)
		{
			ApplyGameplayEffectToTarget(TargetActor, InfiniteEffect);
		}
	}
}

void AMMEffectActor::OnEndOverlap(AActor* TargetActor)
{
	const bool bIsEnemy = TargetActor->ActorHasTag(FName("Enemy"));
	if(bIsEnemy && !bApplyEffectOnEnemies) return;
	
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const auto& InstantEffect : InstantGameplayEffects)
		{
			ApplyGameplayEffectToTarget(TargetActor, InstantEffect);
		}
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const auto& DurationEffect : DurationGameplayEffects)
		{
			ApplyGameplayEffectToTarget(TargetActor, DurationEffect);
		}
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const auto& InfiniteEffect : InfiniteGameplayEffects)
		{
			ApplyGameplayEffectToTarget(TargetActor, InfiniteEffect);
		}
	}

	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetActor)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;

		for (const auto& InfiniteEffect : InfiniteGameplayEffects)
		{
			//Find and Remove the handles that match the target ASC 
			for (auto HandlePair : ActiveEffectHandles)
			{
				if (TargetASC == HandlePair.Value)
				{
					TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
					HandlesToRemove.Add(HandlePair.Key);
				}
			}
		}
		
		for (auto& Handle : HandlesToRemove)
		{
			if(ActiveEffectHandles.Find(Handle)) ActiveEffectHandles.Remove(Handle);
		}
	}
}
