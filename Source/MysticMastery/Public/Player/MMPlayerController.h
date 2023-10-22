// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MMPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API AMMPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMMPlayerController();
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> Context;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	//Tracing stuff under mouse
	void CursorTrace();
	TObjectPtr<IEnemyInterface> LastActorUnderCursor;
	TObjectPtr<IEnemyInterface> CurrentActorUnderCursor;
};
