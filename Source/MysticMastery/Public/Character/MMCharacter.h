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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> Camera;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
