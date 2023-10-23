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
	//This is the same as getting the interface from an actor and cast it, but shorter and fancier
	if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		check(GameplayEffectClass);
		
		FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		const FGameplayEffectSpecHandle GameplayEffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContextHandle);
		TargetASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpecHandle.Data.Get());
	};
}
