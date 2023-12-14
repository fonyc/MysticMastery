// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* Object = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	
	return Object;
}

void UTargetDataUnderMouse::Activate()
{
	if(Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		//We are the client, so we send to the server the cursor data
		SendMouseCursorDataToServer();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		
		//We are on the server, so we need to set a delegate to bind when the activation is made. 
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		//If the data is still not there, then we check the delegate bind. If it has not been broadcast yet, we call SetWaitingOnRemotePlayerData() so it can wait for the data to arrive
		const bool bDelegateHasBeenCalled = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if(!bDelegateHasBeenCalled)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorDataToServer() const
{
	//It opens a prediction window on the server
	FScopedPredictionWindow ScopedPredictionWindow(AbilitySystemComponent.Get());
	
	const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* DataHit = new FGameplayAbilityTargetData_SingleTargetHit();
	DataHit->HitResult = CursorHit;
	DataHandle.Add(DataHit);

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

//This function will be called on the server where the Target Data has been received. In that case we need to make sure 
//that the ASC knows that the data has been received (and thus delete it) and then broadcast that data
void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	//Target data has been received --> Dont keep it cached
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	//Broadcast the valid data if the server has to
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

