
#include "AbilitySystem/MMAttributeSet.h"
#include "Net/UnrealNetwork.h"

UMMAttributeSet::UMMAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.0f);
	InitMana(100.0f);
	InitMaxMana(100.0f);
}

void UMMAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Register Attributes for replication
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMMAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

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
