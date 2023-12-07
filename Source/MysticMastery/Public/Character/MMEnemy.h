// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MMCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "MMEnemy.generated.h"

class UWidgetComponent;

UCLASS()
class MYSTICMASTERY_API AMMEnemy : public AMMCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMMEnemy();

	/** Enemy Interface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** End Enemy Interface */


	/** Combat Interface */
	FORCEINLINE virtual int32 GetPlayerLevel() const override { return Level; }
	/** End Combat Interface */

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Level is not replicated in enemies,it will only be done on the server.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Character Class Defaults")
	int32 Level = 1;
};
