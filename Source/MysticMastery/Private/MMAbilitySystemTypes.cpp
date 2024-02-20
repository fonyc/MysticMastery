#include "MMAbilitySystemTypes.h"

bool FMMGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	//Specify how our struct is going to be serialized through the net
	//We will use a int32 as bool flag, changing its bits to signal if a property is going to be serialized or not
	uint32 RepBits = 0;

#pragma region SAVE BIT FLAGS
	
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid())
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		if (bIsBlockedHit)
		{
			RepBits |= 1 << 7;
		}
		if (bIsCriticalHit)
		{
			RepBits |= 1 << 8;
		}
		if (bIsSuccessfulDebuff)
		{
			RepBits |= 1 << 9;
		}
		if (DebuffDamage > 0.f)
		{
			RepBits |= 1 << 10;
		}
		if (DebuffDuration > 0.f)
		{
			RepBits |= 1 << 11;
		}
		if (DebuffFrequency > 0.f)
		{
			RepBits |= 1 << 12;
		}
		if (DamageType.IsValid())
		{
			RepBits |= 1 << 13;
		}
		if (!DeathImpulseDirection.IsZero())
		{
			RepBits |= 1 << 14;
		}
	}
	
#pragma endregion
	
	Ar.SerializeBits(&RepBits, 14);

	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}

	if (RepBits & (1 << 7))
	{
		Ar << bIsBlockedHit;
	}

	if (RepBits & (1 << 8))
	{
		Ar << bIsCriticalHit;
	}

	if (RepBits & (1 << 9))
	{
		Ar << bIsSuccessfulDebuff;
	}
	
	if (RepBits & (1 << 10))
	{
		Ar << DebuffDamage;
	}

	if (RepBits & (1 << 11))
	{
		Ar << DebuffDuration;
	}

	if (RepBits & (1 << 12))
	{
		Ar << DebuffFrequency;
	}

	if (RepBits & (1 << 13))
	{
		if (Ar.IsLoading())
		{
			if (!DamageType.IsValid())
			{
				DamageType = MakeShared<FGameplayTag>();
			}
		}
		DamageType->NetSerialize(Ar, Map, bOutSuccess);
	}
	
	if (RepBits & (1<<14))
	{
		DeathImpulseDirection.NetSerialize(Ar, Map, bOutSuccess);
	}
	
	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}	
	
	bOutSuccess = true;
	return true;
}
