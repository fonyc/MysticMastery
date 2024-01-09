// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MMAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API AMMAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMMAIController();

protected:
	
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
