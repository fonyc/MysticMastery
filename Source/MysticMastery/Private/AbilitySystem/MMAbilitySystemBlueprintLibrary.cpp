
#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MMAbilitySystemTypes.h"
#include "MMGameplayTags.h"
#include "Game/MMGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "UI/WidgetController/MMWidgetController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MMPlayerState.h"
#include "UI/HUD/MMHUD.h"

bool UMMAbilitySystemBlueprintLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWidgetControllerParams, AMMHUD*& OutMMHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutMMHUD = Cast<AMMHUD>(PC->GetHUD());
		if (OutMMHUD)
		{
			AMMPlayerState* PS = PC->GetPlayerState<AMMPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
				
			OutWidgetControllerParams.AttributeSet = AS;
			OutWidgetControllerParams.PlayerController = PC;
			OutWidgetControllerParams.AbilitySystemComponent = ASC;
			OutWidgetControllerParams.PlayerState = PC->GetPlayerState<AMMPlayerState>();;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UMMAbilitySystemBlueprintLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AMMHUD* MMHUD = nullptr;

	if (MakeWidgetControllerParams(WorldContextObject, WCParams, MMHUD))
	{
		return MMHUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UMMAbilitySystemBlueprintLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AMMHUD* MMHUD = nullptr;

	if (MakeWidgetControllerParams(WorldContextObject, WCParams, MMHUD))
	{
		return MMHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UMMAbilitySystemBlueprintLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AMMHUD* MMHUD = nullptr;

	if (MakeWidgetControllerParams(WorldContextObject, WCParams, MMHUD))
	{
		return MMHUD->GetSpellMenuWidgetController(WCParams);
	}
	return nullptr;
}

void UMMAbilitySystemBlueprintLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 Level, UAbilitySystemComponent* ASC)
{
	if (UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject))
	{
		const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
		const AActor* AvatarActor = ASC->GetAvatarActor();

		FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
		PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
		const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level,PrimaryAttributesContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

		FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
		SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
		const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level,SecondaryAttributesContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

		FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
		VitalAttributesContextHandle.AddSourceObject(AvatarActor);
		const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level,VitalAttributesContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
	}
}

void UMMAbilitySystemBlueprintLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}

	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		int32 PlayerLevel = 1;
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor());
		}
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, PlayerLevel);
		ASC->GiveAbility(AbilitySpec);
	}
}

int32 UMMAbilitySystemBlueprintLibrary::GetXPRewardByClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;

	const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	return static_cast<int32>(Info.XPReward.GetValueAtLevel(CharacterLevel));
}


UCharacterClassInfo* UMMAbilitySystemBlueprintLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AMMGameModeBase* MMGameModeBase = Cast<AMMGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (MMGameModeBase == nullptr) return nullptr;
	return MMGameModeBase->CharacterClassInfo;
}

UAbilityInfo* UMMAbilitySystemBlueprintLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AMMGameModeBase* MMGameModeBase = Cast<AMMGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (MMGameModeBase == nullptr) return nullptr;
	return MMGameModeBase->AbilityInfo;
}

bool UMMAbilitySystemBlueprintLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMMGameplayEffectContext* MMEffectContext = static_cast<const FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MMEffectContext->IsBlockedHit();
	}
	return false;
}

bool UMMAbilitySystemBlueprintLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMMGameplayEffectContext* MMEffectContext = static_cast<const FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MMEffectContext->IsCriticalHit();
	}
	return false;
}

void UMMAbilitySystemBlueprintLibrary::SetCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalHit)
{
	if (FMMGameplayEffectContext* MMEffectContext = static_cast<FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MMEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UMMAbilitySystemBlueprintLibrary::SetBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlockedHit)
{
	if (FMMGameplayEffectContext* MMEffectContext = static_cast<FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MMEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UMMAbilitySystemBlueprintLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float SphereRadius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	// query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(SphereRadius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			AActor* OverlappedActor =  Overlap.GetActor();
			if (OverlappedActor->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(OverlappedActor))
			{
				OutOverlappingActors.AddUnique(OverlappedActor);
			}
		}
	}
}

bool UMMAbilitySystemBlueprintLibrary::IsFriendlyActor(AActor* FirstActor, AActor* SecondActor)
{
	const bool BothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool BothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	return BothAreEnemies || BothArePlayers;
}

FGameplayEffectContextHandle UMMAbilitySystemBlueprintLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FMMGameplayTags& GameplayTags = FMMGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceASC->GetAvatarActor();
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);

	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceASC->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);
	//Damage of the base ability
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);

	//Damage and parameters of the debuff
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	
	DamageEffectParams.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContextHandle;
}
