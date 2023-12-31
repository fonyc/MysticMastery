// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMCharacterBase.h"
#include "MMCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class MYSTICMASTERY_API AMMCharacter : public AMMCharacterBase
{
	GENERATED_BODY()

public:
	AMMCharacter();

#pragma region COMBAT INTERFACE

	virtual int32 GetPlayerLevel() const override;

#pragma endregion

protected:
	UPROPERTY(EditAnywhere, Category="Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> Camera;

public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	virtual void InitializeAbilityActorInfo() override;
};
