// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "MMCharacterBase.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;
class UAnimMontage;
class UNiagaraSystem;

UCLASS()
class MYSTICMASTERY_API AMMCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AMMCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();
	
	/** Combat Interface */
	
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void Die() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	
	UPROPERTY(EditAnywhere, Category= "Combat")
	TArray<FTaggedMontage> AttackMontages;

	UPROPERTY(BlueprintReadOnly, Category= "Combat")
	TMap<FGameplayTag, FName> SocketTagNames;
	
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;

	virtual UNiagaraSystem* GetHitEffect_Implementation() override;
	/** End Combat Interface */
	
protected:
	virtual void BeginPlay() override;

	bool bDead = false;


	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category="Style")
	TObjectPtr<USkeletalMeshComponent> FaceMask;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;

	virtual void InitializeDefaultAttributes() const;

	virtual void InitializeAbilityActorInfo();

	void AddCharacterAbilities();

	/** Death / Dissolve effects */
	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimeline(UMaterialInstanceDynamic* DMI);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DMI);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> CharacterDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="Combat")
	TObjectPtr<USoundBase> DeathSound;
	
	/** End Dissolve effects */

	/** Hit effects */

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="Combat")
	TObjectPtr<UNiagaraSystem> HitEffect;

	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	
	/** End Hit effects */
	
	
private:
	UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
