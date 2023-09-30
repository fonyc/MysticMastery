// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MMPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AMMPlayerController::AMMPlayerController()
{
	bReplicates = true;
}

void AMMPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CursorTrace();
}

void AMMPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(Context);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer());
	check(Subsystem);

	Subsystem->AddMappingContext(Context, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AMMPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMMPlayerController::Move);
}

void AMMPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	//Based in our controller, FW direction is the (0,Controller.Yaw,0) value  
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AMMPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	if(!CursorHit.bBlockingHit) return;

	LastActorUnderCursor = CurrentActorUnderCursor;
	CurrentActorUnderCursor = Cast<IEnemyInterface>(CursorHit.GetActor());
	
	if(LastActorUnderCursor == nullptr && CurrentActorUnderCursor != nullptr)
	{
		CurrentActorUnderCursor->HighlightActor();
	}
	else if(LastActorUnderCursor != nullptr && CurrentActorUnderCursor != nullptr)
	{
		LastActorUnderCursor->UnHighlightActor();
		CurrentActorUnderCursor->HighlightActor();
	}
	else if(LastActorUnderCursor != nullptr && CurrentActorUnderCursor == nullptr)
	{
		LastActorUnderCursor->UnHighlightActor();
	}
}
