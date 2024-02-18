// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMAbilitySystemTypes.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MMAbilitySystemBlueprintLibrary.generated.h"

class AMMHUD;
class UAbilityInfo;
class USpellMenuWidgetController;
class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
struct FWidgetControllerParams;

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMAbilitySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|Widgets", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWidgetControllerParams, AMMHUD*& OutMMHUD);
	
	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|Widgets", meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|Widgets", meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|Widgets", meta = (DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category="MMAbilitySistemBlueprintLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category="MMAbilitySistemBlueprintLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category="MMAbilitySistemBlueprintLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="MMAbilitySistemBlueprintLibrary|AbilityInfo")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|GameplayEffects")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|GameplayEffects")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|GameplayEffects")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|GameplayEffects")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|GameplayEffects")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintCallable, Category="MMAbilitySistemBlueprintLibrary|GameplayEffects")
	static void SetCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalHit);
	
	UFUNCTION(BlueprintCallable, Category="MMAbilitySistemBlueprintLibrary|GameplayEffects")
	static void SetBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category="MMAbilitySistemBlueprintLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors,const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereCenter);

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|GameplayMechanics")
	static bool IsFriendlyActor(AActor* FirstActor, AActor* SecondActor);

	UFUNCTION(BlueprintCallable, Category="MMAbilitySistemBlueprintLibrary|DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);

	UFUNCTION(BlueprintPure, Category="MMAbilitySistemBlueprintLibrary|GameplayMechanics")
	static int32 GetXPRewardByClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);
};
