// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Character/MMCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "MMEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AMMAIController;

UCLASS()
class MYSTICMASTERY_API AMMEnemy : public AMMCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMMEnemy();

	virtual void PossessedBy(AController* NewController) override;

	/** Enemy Interface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** End Enemy Interface */


	/** Combat Interface */
	FORCEINLINE virtual int32 GetPlayerLevel() const override { return Level; }
	virtual void Die() override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	/** End Combat Interface */

	/** Health Component: Callback Functions */
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	/** End Health Component */

	/** Hit React */
	UPROPERTY(BlueprintReadOnly, Category= "Combat")
	bool bHitReacting = false;
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewTagCount);
	
	void MulticastHandleDeath_Implementation() override;
	/** End Hit React */

	UPROPERTY(BlueprintReadOnly, Category= "Combat")
	float BaseWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Combat")
	float LifeSpan = 5.f;

	UPROPERTY(BlueprintReadWrite, Category= "Combat")
	TObjectPtr<AActor> CombatTarget;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	//Level is not replicated in enemies,it will only be done on the server.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviourTree;

	UPROPERTY()
	TObjectPtr<AMMAIController> MMAIController;
};
