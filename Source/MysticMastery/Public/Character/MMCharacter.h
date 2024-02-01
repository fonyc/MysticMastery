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

	virtual int32 GetPlayerLevel_Implementation() const override;

/* End Combat Interface */
	
/* Player Interface */
	
	virtual void AddXP_Implementation(int32 DeltaXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelByXP_Implementation(int32 XP) override;
	virtual int32 GetAttributePointsReward_Implementation(const int32 InLevel) const override;
	virtual int32 GetSpellPointsReward_Implementation(const int32 InLevel) const override;
	virtual void AddAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual void AddSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual void AddPlayerLevel_Implementation(int32 InLevelsToAdd) override;
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
