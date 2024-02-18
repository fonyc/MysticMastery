// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "MMAbilitySystemTypes.h"
#include "MMGameplayTags.h"
#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "MysticMastery/MMLogChannels.h"

struct MMDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	MMDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMAttributeSet, ArmorPenetration, Source, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMAttributeSet, CriticalHitDamage, Source, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMMAttributeSet, PhysicalResistance, Target, false);
	}
};

static const MMDamageStatics& DamageStatics()
{
	static MMDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
}

void UExecCalc_Damage::ApplyDebuffIfAble(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParameters, const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsTodDefs) const
{
	//Check if the damage is caused by a debuff, so we dont apply AGAIN another one 
	if (EvaluationParameters.TargetTags->HasTag(FGameplayTag::RequestGameplayTag(FName("Debuff")))) return;
	
	const FMMGameplayTags& GameplayTags = FMMGameplayTags::Get();
	for (TTuple<FGameplayTag, FGameplayTag> Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType = Pair.Key;
		const FGameplayTag& DebuffType = Pair.Value;
		const float TypeDamage = Spec.GetSetByCallerMagnitude(Pair.Key, false, -1.f);
		if (TypeDamage > -0.5f) //Returns -1.f if it doesnt find anything, but check -0.5f due tu float precision 
		{
			//Determine if there is a successful debuff application
			const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);

			float TargetDebuffResistance = 0.f;
			const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageType];
			//Get the TargetDebuffResistance to the concrete attribute
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InTagsTodDefs[ResistanceTag], EvaluationParameters, TargetDebuffResistance);
			
			//Clamp if we dont wat negative resistances, but if we want more damage for a negative res. then dont
			//TargetDebuffResistance = FMath::Max<float>(0.f,TargetDebuffResistance);
			
			const float EffectiveDebuffChance = SourceDebuffChance * (100 - TargetDebuffResistance) / 100.f;

			//Roll the dice to see if there is debuff application
			if (const bool bDebuff = FMath::RandRange(1,100) < EffectiveDebuffChance)
			{
				UE_LOG(MMLog, Log, TEXT("DEBUF!! %f"), EffectiveDebuffChance);
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

				UMMAbilitySystemBlueprintLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
				UMMAbilitySystemBlueprintLibrary::SetDamageType(ContextHandle, DamageType);
				
				const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.f);
				const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.f);
				const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);
				UMMAbilitySystemBlueprintLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
				UMMAbilitySystemBlueprintLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
				UMMAbilitySystemBlueprintLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
			}
		}
	}
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	
	#pragma region GATHER HIT DATA

	const FMMGameplayTags& Tags = FMMGameplayTags::Get();
	
	//Create a map with Tag-definition so its easier to access them later
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefinitions;
	
	TagsToCaptureDefinitions.Add(Tags.Attributes_Secondary_Armor, DamageStatics().ArmorDef);
	TagsToCaptureDefinitions.Add(Tags.Attributes_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagsToCaptureDefinitions.Add(Tags.Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
		
	TagsToCaptureDefinitions.Add(Tags.Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefinitions.Add(Tags.Attributes_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
	TagsToCaptureDefinitions.Add(Tags.Attributes_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);
		
	TagsToCaptureDefinitions.Add(Tags.Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagsToCaptureDefinitions.Add(Tags.Attributes_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
	TagsToCaptureDefinitions.Add(Tags.Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagsToCaptureDefinitions.Add(Tags.Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);
	
	//Get necessary data
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}

	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluationParameters;
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	
	#pragma endregion
	
	ApplyDebuffIfAble(ExecutionParams, Spec, EvaluationParameters, TagsToCaptureDefinitions);

	#pragma region CALCULATE BASIC DAMAGE WITH RESISTANCES

	float Damage = 0.f;

	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FMMGameplayTags::Get().DamageTypesToResistances)
	{
		TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCapture;
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;
		
		checkf(TagsToCaptureDefinitions.Contains(ResistanceTag), TEXT("TagsCaptureDefinitions doesnt contain Tag: [%s]"), *ResistanceTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDefinition = TagsToCaptureDefinitions[ResistanceTag];
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDefinition, EvaluationParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
		
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false);
		DamageTypeValue *= (100.f - Resistance) / 100.f;

		Damage += DamageTypeValue;
	}
	
	#pragma endregion

	#pragma region BLOCK

	//Capture block chance on target and determine 
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters,TargetBlockChance);
	TargetBlockChance = FMath::Max(TargetBlockChance, 0.f);

	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

	//Set is blocked hit, whether it is or not
	UMMAbilitySystemBlueprintLibrary::SetBlockedHit(EffectContextHandle, bBlocked);

	if (bBlocked)
	{
		Damage *= .5f;
	}
	# pragma endregion

	# pragma region ARMOR PENETRATION

	//Get Armor and Armor penetration. Armor penetration ignores a % of target Armor.
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters,TargetArmor);
	TargetArmor = FMath::Max(TargetArmor, 0.f);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(SourceArmorPenetration, 0.f);

	//Get Armor Coefficient
	const UCharacterClassInfo* CharacterClassInfo = UMMAbilitySystemBlueprintLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageExecCalcCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);

	//Get EffectiveArmor Coefficient
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageExecCalcCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);

	//Apply the damage % modifications 
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	# pragma endregion

	#pragma region CRITICAL HIT

	//Take Target Critical Hit Resistance 
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max(TargetCriticalHitResistance, 0.f);

	// Take Critical Hit Damage from Source
	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

	//Take Critical Hit Chance from Source
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
	SourceCriticalHitChance = FMath::Min(SourceCriticalHitChance, 100.f);

	//Take Critical Hit Resistance Coefficient
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageExecCalcCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

	//Check if there is critical hit
	const float EffectiveCriticalChance = FMath::Max(SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient, 0.f);
	const bool bCritical = FMath::RandRange(1, 100) < EffectiveCriticalChance;

	//Set critical hit in the Effect Context Handle whether is critical hit or not
	UMMAbilitySystemBlueprintLibrary::SetCriticalHit(EffectContextHandle, bCritical);
	
	if (bCritical)
	{
		Damage = (Damage * 2.f) + SourceCriticalHitDamage;
	}
	
	#pragma endregion

	const FGameplayModifierEvaluatedData EvaluatedData(UMMAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
