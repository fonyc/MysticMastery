// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MMCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/MMPlayerState.h"


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
	AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);

	AbilitySystemComponent = MMPlayerState->GetAbilitySystemComponent();
	AttributeSet = MMPlayerState->GetAttributeSet();
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(MMPlayerState, this);
}
