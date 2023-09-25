// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MMCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
AMMCharacter::AMMCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//Rotate character to face movement direction
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f,400.0f,0.0f);
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

void AMMCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMMCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

