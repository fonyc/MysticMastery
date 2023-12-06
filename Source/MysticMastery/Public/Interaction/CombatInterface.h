#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYSTICMASTERY_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual int32 GetPlayerLevel() const;
	virtual FVector GetCombatSocketLocation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateFacingWarpTarget(const FVector& Target);
};
