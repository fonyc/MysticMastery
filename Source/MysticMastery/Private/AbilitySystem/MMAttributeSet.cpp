#include "AbilitySystem/MMAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "MMGameplayTags.h"
#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/MMPlayerController.h"

UMMAttributeSet::UMMAttributeSet()
{
	const FMMGameplayTags& GameplayTags = FMMGameplayTags::Get();

	/* -- Primary Attributes -- */
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

	/* -- Secondary Attributes -- */
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
	
	/* -- Secondary Attributes (Resistances) -- */
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
}

//Register attributes for replication
void UMMAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/* -- Primary -- */
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	/* -- Secondary -- */
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);

	/* -- Resistances -- */
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	

	/* -- Vital -- */
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

/*
 * Function kicks in before changing any attribute
 */
void UMMAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	//Pre-Clamp attributes 
	if (Attribute == GetHealthAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	if (Attribute == GetManaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
}

void UMMAttributeSet::ShowFloatingText(const FEffectProperties& Props,float Damage, bool bBlockedHit, bool bCriticalHit)
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		//Damage causer is PC
		if (AMMPlayerController* PC = Cast<AMMPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}

		//Damage Causer is Enemy AI
		if (AMMPlayerController* PC = Cast<AMMPlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

void UMMAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
		const ECharacterClass TargetCharacterClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		const int32 XPReward = UMMAbilitySystemBlueprintLibrary::GetXPRewardByClassAndLevel(Props.TargetCharacter, TargetCharacterClass, TargetLevel);

		const FMMGameplayTags& GameplayTags = FMMGameplayTags::Get();
		FGameplayEventData Payload;

		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;
		//Send the exp to the source character, meaning, the one causing damage
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
	}
}

void UMMAttributeSet::HandleIncomingDamage(FEffectProperties Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
		
	if (LocalIncomingDamage > 0 )
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		if (const bool bIsFatalDamage = NewHealth <= 0.f)
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor))
			{
				CombatInterface->Die();
			}
			SendXPEvent(Props);
		}
		else
		{
			//Ability just caused damage
			//Activate an Ability by using a tag related to it
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FMMGameplayTags::Get().Effects_HitReact);
			Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
		}

		//Call the text damage on top of the character
		const bool bBlocked = UMMAbilitySystemBlueprintLibrary::IsBlockedHit(Props.EffectContextHandle);
		const bool bCriticalHit = UMMAbilitySystemBlueprintLibrary::IsCriticalHit(Props.EffectContextHandle);
		ShowFloatingText(Props, LocalIncomingDamage, bBlocked, bCriticalHit);

		if (UMMAbilitySystemBlueprintLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		{
			//Apply the the debuff
			ApplyDebuff(Props);
		}
			
	}
}

void UMMAttributeSet::HandleIncomingXP(FEffectProperties Props)
{
	const float LocalIncomingXP = GetIncomingXP();
	SetIncomingXP(0.f);

	// Remember SourceCharacter is the owner (GA_ListenForEvents applies GE_EventBased, adding to Incoming XP to himself)
	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
	{
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

		const int32 NewLevel = IPlayerInterface::Execute_FindLevelByXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
		const int32 TimesLeveledUp = NewLevel - CurrentLevel;

		//Player Leveled up 1+ times
		if (TimesLeveledUp > 0)
		{
			const int32 AttributePointsAwarded = IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter, CurrentLevel);
			const int32 SpellPointsAwarded = IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceCharacter, CurrentLevel);

			//Add Level, Spell Points and Attribute Points to spend
			IPlayerInterface::Execute_AddPlayerLevel(Props.SourceCharacter, TimesLeveledUp);
			IPlayerInterface::Execute_AddAttributePoints(Props.SourceCharacter, AttributePointsAwarded);
			IPlayerInterface::Execute_AddSpellPoints(Props.SourceCharacter, SpellPointsAwarded);

			//Raise flags to restore mana and health inside PostAttributeChange
			bRestoreHealth = true;
			bRestoreMana = true;

			// Perform additional actions inside the character when leveling up (specially Aesthetics)
			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
		}
			
		IPlayerInterface::Execute_AddXP(Props.SourceCharacter, LocalIncomingXP);
	}
}

void UMMAttributeSet::ApplyDebuff(FEffectProperties Props)
{
	const FMMGameplayTags GameplayTags = FMMGameplayTags::Get();
	
	FGameplayEffectContextHandle EffectContextHandle = Props.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(Props.SourceAvatarActor);

	const FGameplayTag DamageType = UMMAbilitySystemBlueprintLibrary::GetDamageType(Props.EffectContextHandle);
	const float DebuffDuration = UMMAbilitySystemBlueprintLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffDamage = UMMAbilitySystemBlueprintLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffFrequency = UMMAbilitySystemBlueprintLibrary::GetDebuffFrequency(Props.EffectContextHandle);
	const FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect* DebuffEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

	DebuffEffect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DebuffEffect->Period = DebuffFrequency;
	DebuffEffect->bExecutePeriodicEffectOnApplication = false;
	DebuffEffect->DurationMagnitude = FScalableFloat(DebuffDuration);
	DebuffEffect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	DebuffEffect->StackLimitCount = 1;
	
	//Add calculation type
	FGameplayEffectExecutionDefinition Execution;
	Execution.CalculationClass = UExecCalc_Damage::StaticClass();
	DebuffEffect->Executions.Add(Execution);

	/** -- Start Add tag -- */
	
	const FGameplayTag DebuffType = GameplayTags.DamageTypesToDebuffs[DamageType];
	FInheritedTagContainer TagContainer = FInheritedTagContainer();
	TagContainer.Added.AddTag(DebuffType); 

	// Create and add the component (a tag container with inheritable tags in this case) to the GE
	UTargetTagsGameplayEffectComponent& TargetTagsComponent = DebuffEffect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>(); 
	TargetTagsComponent.SetAndApplyTargetTagChanges(TagContainer);
	
	/** -- End Add tag -- */

	const int NumModifiers = DebuffEffect->Modifiers.Num();
	DebuffEffect->Modifiers.Add(FGameplayModifierInfo());
	
	FGameplayModifierInfo& ModifierInfo = DebuffEffect->Modifiers[NumModifiers];
	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = GetIncomingDamageAttribute();

	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(DebuffEffect, EffectContextHandle, Props.EffectContextHandle.GetAbilityLevel()))
	{
		MutableSpec->SetSetByCallerMagnitude(DamageType, DebuffDamage);
 
		FMMGameplayEffectContext* MMContext = static_cast<FMMGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		MMContext->SetDamageType(DebuffDamageType);
 
		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
}

//Function kicks in after the GE has been executed
void UMMAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	//Dont apply anything if the character is already dead
	if (Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter))return;
	
	//Handle Incoming Attribute 
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
	
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
}

void UMMAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	//The Max health changed AND RestoreHealth is true cause we leveled up
	if (Attribute == GetMaxHealthAttribute() && bRestoreHealth)
	{
		SetHealth(GetMaxHealth());
		bRestoreHealth = false;
	}
	if (Attribute == GetMaxManaAttribute() && bRestoreMana)
	{
		SetMana(GetMaxMana());
		bRestoreMana = false;
	}
}

#pragma region ONREP METHODS

void UMMAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, Health, OldHealth);
}

void UMMAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, MaxHealth, OldMaxHealth);
}

void UMMAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, Mana, OldMana);
}

void UMMAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, MaxMana, OldMaxMana);
}

void UMMAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, Strength, OldStrength);
}

void UMMAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, Intelligence, OldIntelligence);
}

void UMMAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, Resilience, OldResilience);
}

void UMMAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, Vigor, OldVigor);
}

void UMMAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, Armor, OldArmor);
}

void UMMAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UMMAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, BlockChance, OldBlockChance);
}

void UMMAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UMMAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UMMAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UMMAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UMMAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UMMAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, ManaRegeneration, OldFireResistance);
}

void UMMAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, ManaRegeneration, OldArcaneResistance);
}

void UMMAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, ManaRegeneration, OldLightningResistance);
}

void UMMAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMMAttributeSet, ManaRegeneration, OldPhysicalResistance);
}

#pragma endregion

//Fill the FEffectProperties struct with useful data
void UMMAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	//Source = causer of the effect, Target = target of the effect (owner of this ASC)

	Props.EffectContextHandle = Data.EffectSpec.GetContext();

	//Returns the ASC from the handle
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo-> AvatarActor.IsValid())
	{
		//Get the source Avatar Actor from ASC
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();

		//Get the player controller from source
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();

		//Try to get the PC from the pawn directly (if its impossible from source)
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}

		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
		//Props.TargetASC = Data.Target.AbilityActorInfo->AbilitySystemComponent.Get();
	}
}