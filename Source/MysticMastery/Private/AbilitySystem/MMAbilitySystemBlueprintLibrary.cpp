
#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MMAbilitySystemTypes.h"
#include "MMGameplayTags.h"
#include "Game/MMGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "UI/WidgetController/MMWidgetController.h"
#include "Kismet/GameplayStatics.h"
#include "MysticMastery/MMLogChannels.h"
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

bool UMMAbilitySystemBlueprintLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMMGameplayEffectContext* MMEffectContext = static_cast<const FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MMEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UMMAbilitySystemBlueprintLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMMGameplayEffectContext* MMEffectContext = static_cast<const FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MMEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UMMAbilitySystemBlueprintLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMMGameplayEffectContext* MMEffectContext = static_cast<const FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MMEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UMMAbilitySystemBlueprintLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMMGameplayEffectContext* MMEffectContext = static_cast<const FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MMEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UMMAbilitySystemBlueprintLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMMGameplayEffectContext* MMEffectContext = static_cast<const FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return *MMEffectContext->GetDamageType();
	}
	return FGameplayTag();
}

FVector UMMAbilitySystemBlueprintLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMMGameplayEffectContext* MMEffectContext = static_cast<const FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MMEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UMMAbilitySystemBlueprintLibrary::GetKnockBackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMMGameplayEffectContext* MMEffectContext = static_cast<const FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MMEffectContext->GetKnockBackForce();
	}
	return FVector::ZeroVector;
}

float UMMAbilitySystemBlueprintLibrary::GetKnockBackChance(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMMGameplayEffectContext* MMEffectContext = static_cast<const FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return MMEffectContext->GetKnockBackChance();
	}
	return 0.f;
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

void UMMAbilitySystemBlueprintLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsSuccessfulDebuff)
{
	if (FMMGameplayEffectContext* MMEffectContext = static_cast<FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MMEffectContext->SetIsSuccessfulDebuff(bInIsSuccessfulDebuff);
	}
}

void UMMAbilitySystemBlueprintLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, const float InDamage)
{
	if (FMMGameplayEffectContext* MMEffectContext = static_cast<FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MMEffectContext->SetDebuffDamage(InDamage);
	}
}

void UMMAbilitySystemBlueprintLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, const float InDuration)
{
	if (FMMGameplayEffectContext* MMEffectContext = static_cast<FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MMEffectContext->SetDebuffDuration(InDuration);
	}
}

void UMMAbilitySystemBlueprintLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, const float InFrequency)
{
	if (FMMGameplayEffectContext* MMEffectContext = static_cast<FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MMEffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UMMAbilitySystemBlueprintLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FMMGameplayEffectContext* MMEffectContext = static_cast<FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		MMEffectContext->SetDamageType(DamageType);
	}
}

void UMMAbilitySystemBlueprintLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse)
{
	if (FMMGameplayEffectContext* MMEffectContext = static_cast<FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MMEffectContext->SetDeathImpulse(InDeathImpulse);
	}
}

void UMMAbilitySystemBlueprintLibrary::SetKnockBackForce(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockBackForce)
{
	if (FMMGameplayEffectContext* MMEffectContext = static_cast<FMMGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		MMEffectContext->SetKnockBackForce(InKnockBackForce);
	}
}

void UMMAbilitySystemBlueprintLibrary::GetLivePlayersWithinRadius(
	const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors,
	const TArray<AActor*>& ActorsToIgnore,
	float SphereRadius,
	const FVector& SphereOrigin)
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

void UMMAbilitySystemBlueprintLibrary::GetClosestTargets(int32 NumTargets, const TArray<AActor*>& Actors, TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Actors.IsEmpty()) return;

	TArray<AActor*> SortedActors = Actors;
	SortedActors.Sort([Origin](const AActor& A, const AActor& B)
	{
		const double DistanceA = (A.GetActorLocation() - Origin).Length();
		const double DistanceB = (B.GetActorLocation() - Origin).Length();
		
		return DistanceA < DistanceB;
	});

	for (int x = 0; x < FMath::Min(NumTargets, SortedActors.Num()); x++)
	{
		OutClosestTargets.AddUnique(SortedActors[x]);
	}
}

bool UMMAbilitySystemBlueprintLibrary::IsFriendlyActor(AActor* FirstActor, AActor* SecondActor)
{
	if (IsValid(FirstActor) && IsValid(SecondActor))
	{
		const bool BothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
		const bool BothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
		return BothAreEnemies || BothArePlayers;
	}
	return false;
}

FGameplayEffectContextHandle UMMAbilitySystemBlueprintLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FMMGameplayTags& GameplayTags = FMMGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceASC->GetAvatarActor();
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulseVector);
	const bool bIsKnockBackHit = FMath::RandRange(1.f,100.f) < DamageEffectParams.KnockBackChance;
	const FVector FinalKnockBackForce = bIsKnockBackHit ? DamageEffectParams.KnockBackForceVector : FVector::ZeroVector;
	SetKnockBackForce(EffectContextHandle, FinalKnockBackForce);

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

TArray<FRotator> UMMAbilitySystemBlueprintLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, const float Spread, const int32 NumRotators)
{
	TArray<FRotator> Rotators;
	const FVector LeftSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);

	if (NumRotators > 1)
	{
		const float DeltaSpread = Spread / (NumRotators - 1);
		for (int32 x = 0; x < NumRotators; x++)
		{
			const FVector Direction = LeftSpread.RotateAngleAxis(DeltaSpread * x, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UMMAbilitySystemBlueprintLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, const float Spread,  const int32 NumVectors)
{
	TArray<FVector> Vectors;
	const FVector LeftSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);

	if (NumVectors > 1)
	{
		const float DeltaSpread = Spread / (NumVectors - 1);
		for (int32 x = 0; x < NumVectors; x++)
		{
			const FVector Direction = LeftSpread.RotateAngleAxis(DeltaSpread * x, FVector::UpVector);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}
	return Vectors;
}
