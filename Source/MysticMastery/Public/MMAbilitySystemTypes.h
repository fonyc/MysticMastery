#pragma once

#include "GameplayEffectTypes.h"
#include "MMAbilitySystemTypes.generated.h"

USTRUCT(BlueprintType)
struct FMMGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }

	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual UScriptStruct* GetScriptStruct() const
	{
		return StaticStruct();
	}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FMMGameplayEffectContext* Duplicate() const
	{
		FMMGameplayEffectContext* NewContext = new FMMGameplayEffectContext();
		*NewContext = *this;
		if(GetHitResult())
		{
			//Deep copy of hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

protected:
	UPROPERTY()
	bool bIsCriticalHit = false;
	
	UPROPERTY()
	bool bIsBlockedHit = false;
};

template<>
struct TStructOpsTypeTraits<FMMGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FMMGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};