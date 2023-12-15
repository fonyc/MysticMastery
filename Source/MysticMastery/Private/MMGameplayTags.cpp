// Fill out your copyright notice in the Description page of Project Settings.


#include "MMGameplayTags.h"
#include "GameplayTagsManager.h"

FMMGameplayTags FMMGameplayTags::GameplayTags;

void FMMGameplayTags::InitializeNativeGameplayTags()
{

	//Attributes.Primary
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"), FString("Increases physical damage"));

	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.Primary.Intelligence"), FString("Increases magical damage"));

	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"), FString("Increases Health"));

	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"), FString("Increases Armor and armor penetration"));

	
	//Attributes.Secondary
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"), FString("Reduces damage taken and improves Block Chance"));

	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.ArmorPenetration"), FString("Ignored percentage of enemy's armor, increases critical hit chance"));

	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.BlockChance"), FString("Chance to reduce incoming damage by half"));
	
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.CriticalHitChance"), FString("Chance to double damage dealt plus the critical hit bonus"));
	
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.CriticalHitDamage"), FString("Bonus damage added when a critical hit is performed"));

	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.CriticalHitResistance"), FString("Reduces Critical Hit Chance of attacking enemies"));

	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.HealthRegeneration"), FString("Quantity of Health regenerated per second"));

	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.ManaRegeneration"), FString("Quantity of mana regenerated per second"));

	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.MaxHealth"), FString("Maximum quantity of health"));

	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.MaxMana"), FString("Maximum quantity of mana"));

	//InputTags
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.LMB"), FString("Input Tag for Left Mouse Button"));

	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.RMB"), FString("Input Tag for Right Mouse Button"));

	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.1"), FString("Input Tag for the Num1 key"));

	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.2"), FString("Input Tag for the Num2 key"));

	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.3"), FString("Input Tag for the Num3 key"));

	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.4"), FString("Input Tag for the Num4 key"));

	//Miscelaneous
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage"), FString("Input Tag for the meta attribute damage"));

	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Effects.HitReact"), FString("Tag added when some Actor is Hit by an attack"));
}
