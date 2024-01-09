// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "MMEffectActor.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};

UCLASS()
class MYSTICMASTERY_API AMMEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AMMEffectActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyGameplayEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | General")
	bool bDestroyOnEffectApplication;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | General")
	bool bApplyEffectOnEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | General")
	float ActorLevel = 1.f;

	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;

#pragma region APPLIED EFFECTS

	//Instant
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | Instant Effects")
	TArray<TSubclassOf<UGameplayEffect>> InstantGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | Instant Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | Instant Effects")
	EEffectRemovalPolicy InstantEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;;

	//Duration
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | Duration Effects")
	TArray<TSubclassOf<UGameplayEffect>> DurationGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | Duration Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | Duration Effects")
	EEffectRemovalPolicy DurationEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;;

	//Infinite 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | Infinite Effects")
	TArray<TSubclassOf<UGameplayEffect>> InfiniteGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | Infinite Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects | Infinite Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

#pragma endregion
	
};
