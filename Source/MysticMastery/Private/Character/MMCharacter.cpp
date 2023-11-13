// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MMCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/MMPlayerController.h"
#include "Player/MMPlayerState.h"
#include "UI/HUD/MMHUD.h"


// Sets default values
AMMCharacter::AMMCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//Rotate character to face movement direction
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);

	SpringArmComponent->TargetArmLength = 750.0f;
	SpringArmComponent->bUsePawnControlRotation = false;

	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bInheritYaw = false;


	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArmComponent);
	Camera->bUsePawnControlRotation = false;
}

int32 AMMCharacter::GetPlayerLevel() const 
{
	const AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);
	return MMPlayerState->GetPlayerLevel();
}

void AMMCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Init ability actor info for the SERVER 
	InitializeAbilityActorInfo();
}

void AMMCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//Init Ability actor info for the CLIENT
	InitializeAbilityActorInfo();
}

void AMMCharacter::InitializeAbilityActorInfo()
{
	Super::InitializeAbilityActorInfo();
	
	AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);

	MMPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(MMPlayerState, this);
	
	//Warn the delegate we just set the ability actor info
	Cast<UMMAbilitySystemComponent>(MMPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

	AbilitySystemComponent = MMPlayerState->GetAbilitySystemComponent();
	AttributeSet = MMPlayerState->GetAttributeSet();
	

	//This is a perfect spot to fill the HUD data, cause we have all 4 variables needed (PS,PC,ASC,AC) and also access to the HUD. 
	//Only in the server all PC are valid --> checking the PC cannot stop the execution (local player1 will stop its
	//execution when checking Player2's PC cause they dont have it)--> So we just check if it's valid
	if(AMMPlayerController* MMPlayerController = Cast<AMMPlayerController>(GetController()))
	{
		if(AMMHUD* MMHUD = Cast<AMMHUD>(MMPlayerController->GetHUD()))
		{
			MMHUD->InitOverlay(MMPlayerController,MMPlayerState,AbilitySystemComponent,AttributeSet);
		}
	}

	//Give initial values to the attributes. We do it just on the Server since its marked to be replicated
	InitializeDefaultAttributes();
}
