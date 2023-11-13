// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MMCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "MMEnemy.generated.h"

UCLASS()
class MYSTICMASTERY_API AMMEnemy : public AMMCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMMEnemy();

#pragma region ENEMY INTERFACE

	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

#pragma endregion

#pragma region COMBAT INTERFACE

	FORCEINLINE virtual int32 GetPlayerLevel() const override { return Level; }

#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Level is not replicated in enemies,it will only be done on the server.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Character Class Defaults")
	int32 Level = 1;
};
