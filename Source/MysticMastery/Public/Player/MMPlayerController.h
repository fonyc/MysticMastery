// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MMPlayerController.generated.h"

class UNiagaraSystem;
class UDamageTextComponent;
struct FGameplayTag;
struct FInputActionValue;
class UMMInputConfig;
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
class UMMAbilitySystemComponent;
class USplineComponent;

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

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> Context;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;

	FORCEINLINE void ShiftPressed() { bShiftKeyDown = true; }
	FORCEINLINE void ShiftReleased() { bShiftKeyDown = false; }
	bool bShiftKeyDown = false;

	void Move(const FInputActionValue& InputActionValue);

	//Tracing stuff under mouse
	FHitResult CursorHit;
	void CursorTrace();
	IEnemyInterface* LastActorUnderCursor;
	IEnemyInterface* CurrentActorUnderCursor;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UMMInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UMMAbilitySystemComponent> MMAbilitySystemComponent;

	UMMAbilitySystemComponent* GetAbilitySystemComponent();

	//Destination stored after we click with the mouse
	FVector CachedDestination = FVector::ZeroVector;

	//Time that character have been following the mouse cursor
	float TimeFollowingCursor = 0.f;

	//Amount of time needed to pass before a short press (move to click) is considered long press (auto move) 
	float ShortPressThreshold = 0.5f;
	
	//Flag used to determine if the AutoRunning is on/off
	bool bAutoRunning = false;

	//Flag that determines if the cursor is on some Actor with the IEnemyInterface
	bool bTargeting = false;

	//Function that takes care of running
	void AutoRun();

	//Min Distance which we consider the character has arrived destination 
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	//Damage Text
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};
