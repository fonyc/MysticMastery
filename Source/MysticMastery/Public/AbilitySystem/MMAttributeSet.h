// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MMAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){};

	FGameplayEffectContextHandle EffectContextHandle;
	
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;

};

//we need the FGameplayAttribute() signature, but to make it generic we use TStaticFuncPtr
//real signature would be TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultUserPolicy>::FFuncPtr FAttributeFuncPtr;
template <class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T,FDefaultDelegateUserPolicy>::FFuncPtr;

/**
 * 
 */

UCLASS()
class MYSTICMASTERY_API UMMAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMMAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	//Maps every given tag to its FGameplayAttribute (in the form of a function pointer that points a static function)
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;

#pragma region PRIMARY ATTRIBUTES
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category= "Primary Attributes")
	FGameplayAttributeData Strength;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category= "Primary Attributes")
	FGameplayAttributeData Intelligence;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category= "Primary Attributes")
	FGameplayAttributeData Resilience;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category= "Primary Attributes")
	FGameplayAttributeData Vigor;


#pragma endregion

#pragma region SECONDARY ATTRIBUTES
	
#pragma region SPECIAL ATTRIBUTES

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LifeSteal, Category= "Special Attributes")
	FGameplayAttributeData LifeSteal;
	
#pragma endregion 
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category= "Secondary Attributes")
	FGameplayAttributeData Armor;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category= "Secondary Attributes")
	FGameplayAttributeData ArmorPenetration;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category= "Secondary Attributes")
	FGameplayAttributeData BlockChance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitChance, Category= "Secondary Attributes")
	FGameplayAttributeData CriticalHitChance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category= "Secondary Attributes")
	FGameplayAttributeData CriticalHitDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitResistance, Category= "Secondary Attributes")
	FGameplayAttributeData CriticalHitResistance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category= "Secondary Attributes")
	FGameplayAttributeData HealthRegeneration;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegeneration, Category= "Secondary Attributes")
	FGameplayAttributeData ManaRegeneration;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category= "Vital Attributes")
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category= "Vital Attributes")
	FGameplayAttributeData MaxMana;

	/**
	 * There are also secondary attributes, but they are separated just for clarity
	 */

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category= "Resistance Attributes")
	FGameplayAttributeData FireResistance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArcaneResistance, Category= "Resistance Attributes")
	FGameplayAttributeData ArcaneResistance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category= "Resistance Attributes")
	FGameplayAttributeData LightningResistance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalResistance, Category= "Resistance Attributes")
	FGameplayAttributeData PhysicalResistance;
	
#pragma  endregion
	
#pragma region VITAL ATTRIBUTES
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category= "Vital Attributes")
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category= "Vital Attributes")
	FGameplayAttributeData Mana;
	
#pragma endregion

#pragma region META ATTRIBUTES

	//Meta attributes are not replicated
	UPROPERTY(BlueprintReadOnly, Category= "Meta Attributes")
	FGameplayAttributeData IncomingDamage;

	UPROPERTY(BlueprintReadOnly, Category= "Meta Attributes")
	FGameplayAttributeData IncomingXP;
	
#pragma endregion
	
#pragma region ATTRIBUTE ACCESSORS
	//Macro Attribute Accessors 
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, Strength);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, Intelligence);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, Resilience);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, Vigor);

	ATTRIBUTE_ACCESSORS(UMMAttributeSet, LifeSteal);
	
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, Mana);
	
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, MaxMana);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, Armor);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, ArmorPenetration);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, BlockChance);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, CriticalHitChance);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, CriticalHitDamage);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, CriticalHitResistance);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, HealthRegeneration);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, ManaRegeneration);
	
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, FireResistance);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, ArcaneResistance);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, LightningResistance);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, PhysicalResistance);
	
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, IncomingDamage);
	ATTRIBUTE_ACCESSORS(UMMAttributeSet, IncomingXP);
	
#pragma endregion
	
#pragma region ONREP METHOD DECLARATIONS

	UFUNCTION()
	void OnRep_LifeSteal(const FGameplayAttributeData& OldLifeSteal) const;
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;

	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;

	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;

	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;

	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;

	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;

	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;

	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;

	UFUNCTION()
	void OnRep_FireResistance (const FGameplayAttributeData& OldFireResistance) const;

	UFUNCTION()
	void OnRep_ArcaneResistance (const FGameplayAttributeData& OldArcaneResistance) const;

	UFUNCTION()
	void OnRep_LightningResistance (const FGameplayAttributeData& OldLightningResistance) const;

	UFUNCTION()
	void OnRep_PhysicalResistance (const FGameplayAttributeData& OldPhysicalResistance) const;
	
#pragma endregion
	
private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	static void ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit);
	void SendXPEvent(const FEffectProperties& Props);
	void HealDamageInstigator(const FEffectProperties& Props, const float LifeToRecover);
	void HandleIncomingDamage(const FEffectProperties& Props);
	void HandleIncomingXP(FEffectProperties Props);
	void ApplyDebuff(FEffectProperties Props);
	bool bRestoreHealth = false;
	bool bRestoreMana = false;
};
