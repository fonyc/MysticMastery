// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "MMCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class MYSTICMASTERY_API AMMCharacter : public AMMCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AMMCharacter();

/* Combat Interface */

	virtual int32 GetPlayerLevel() const override;

/* End Combat Interface */
	
/* Player Interface */
	
	virtual void AddXP_Implementation(int32 DeltaXP) override;
	
/* End Player Interface */

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
