// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MMAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AMMAIController::AMMAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	check(Blackboard);
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviourTreeComponent");
	check(BehaviorTreeComponent);
}
