#pragma once

#include "GameplayEffectTypes.h"
#include "MMAbilitySystemTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams()
	{
	};

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0;

	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulseVector = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite)
	float KnockBackChance = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float KnockBackForceMagnitude = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	FVector KnockBackForceVector = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FMMGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsSuccessfulDebuff() const { return bIsSuccessfulDebuff; }
	float GetDebuffDamage() const { return DebuffDamage; }
	float GetDebuffFrequency() const { return DebuffFrequency; }
	float GetDebuffDuration() const { return DebuffDuration; }
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }
	FVector GetDeathImpulse() const { return DeathImpulseDirection; } 
	FVector GetKnockBackForce() const { return KnockBackForceVector; } 
	float GetKnockBackChance() const { return KnockBackChance; }

	void SetIsCriticalHit(const bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetIsSuccessfulDebuff(const bool bInIsSuccessfulDebuff) { bIsSuccessfulDebuff = bInIsSuccessfulDebuff; }
	void SetDebuffDamage(const float NewDamage) { DebuffDamage = NewDamage; }
	void SetDebuffFrequency(const float NewFrequency) { DebuffFrequency = NewFrequency; }
	void SetDebuffDuration(const float NewDuration) { DebuffDuration = NewDuration; }
	void SetDamageType(const TSharedPtr<FGameplayTag>& InDamageType) { DamageType = InDamageType; }
	void SetDeathImpulse(const FVector& InImpulse) { DeathImpulseDirection = InImpulse; }
	void SetKnockBackForce(const FVector& InKnockBackForce) { KnockBackForceVector = InKnockBackForce; }
	void SetKnockBackChance(const float InKnockBackChance) { KnockBackChance = InKnockBackChance; }

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
		if (GetHitResult())
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

	UPROPERTY()
	bool bIsSuccessfulDebuff = false;

	UPROPERTY()
	float DebuffDamage = 0.f;

	UPROPERTY()
	float DebuffDuration = 0.f;

	UPROPERTY()
	float DebuffFrequency = 0.f;

	TSharedPtr<FGameplayTag> DamageType;

	UPROPERTY()
	FVector DeathImpulseDirection = FVector::ZeroVector;

	UPROPERTY()
	float KnockBackChance = 0.f;
	
	UPROPERTY()
	FVector KnockBackForceVector = FVector::ZeroVector;
	
};

template <>
struct TStructOpsTypeTraits<FMMGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FMMGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
