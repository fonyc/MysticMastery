// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* Object = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);

	
	
	return Object;
}

void UTargetDataUnderMouse::Activate()
{
	//We dont want to call super since the inherited version performs a log and nothing else
	const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult Hit;
	PC->GetHitResultUnderCursor(ECC_Visibility,false,Hit);
	ValidData.Broadcast(Hit.Location);
}
