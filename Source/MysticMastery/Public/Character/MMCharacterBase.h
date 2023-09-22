// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MMCharacterBase.generated.h"

UCLASS()
class MYSTICMASTERY_API AMMCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AMMCharacterBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

};
