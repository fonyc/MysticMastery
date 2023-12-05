// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MMPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "MMGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/MMInputComponent.h"
#include "Interaction/EnemyInterface.h"

AMMPlayerController::AMMPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AMMPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CursorTrace();
	AutoRun();
}

void AMMPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(Context);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(Context, 0);
	}

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

	UMMInputComponent* MMInputComponent = CastChecked<UMMInputComponent>(InputComponent);
	MMInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMMPlayerController::Move);
	
	MMInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AMMPlayerController::ShiftPressed);
	MMInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AMMPlayerController::ShiftReleased);
	
	MMInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AMMPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	//Based in our controller, FW direction is the (0, Controller.Yaw, 0) value  
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AMMPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActorUnderCursor = CurrentActorUnderCursor;
	CurrentActorUnderCursor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (LastActorUnderCursor != CurrentActorUnderCursor)
	{
		if (LastActorUnderCursor) LastActorUnderCursor->UnHighlightActor();
		if (CurrentActorUnderCursor) CurrentActorUnderCursor->HighlightActor();
	}
}

void AMMPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTag(FMMGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = CurrentActorUnderCursor ? true : false;
		bAutoRunning = false;
	}
}

void AMMPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//We are pressing RMB
	if (!InputTag.MatchesTagExact(FMMGameplayTags::Get().InputTag_LMB))
	{
		if (GetAbilitySystemComponent()) GetAbilitySystemComponent()->AbilityInputReleased(InputTag);
		return;
	}
	
	if (GetAbilitySystemComponent()) GetAbilitySystemComponent()->AbilityInputReleased(InputTag);
	
	//We are pressing LMB but we are targeting an enemy --> Cast Ability
	if (!bTargeting && !bShiftKeyDown)
	{
		if (const APawn* ControlledPawn = GetPawn(); TimeFollowingCursor <= ShortPressThreshold)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();
				int32 index = 0;
				
				for (const FVector& PointLocation : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLocation, ESplineCoordinateSpace::World);
				}
				
				if (NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
				}
				bAutoRunning = true;
			}
		}
		TimeFollowingCursor = 0.f;
		bTargeting = false;
	}
}

void AMMPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//We are pressing RMB
	if (!InputTag.MatchesTagExact(FMMGameplayTags::Get().InputTag_LMB))
	{
		if (GetAbilitySystemComponent())GetAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
		return;
	}

	//We are pressing LMB but we are targeting an enemy --> Cast Ability
	if (bTargeting || bShiftKeyDown)
	{
		if (GetAbilitySystemComponent()) GetAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
	}
	//We press LMB without target --> We are moving
	else
	{
		TimeFollowingCursor += GetWorld()->GetDeltaSeconds();

		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UMMAbilitySystemComponent* AMMPlayerController::GetAbilitySystemComponent()
{
	if (MMAbilitySystemComponent == nullptr)
	{
		MMAbilitySystemComponent = Cast<UMMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return MMAbilitySystemComponent;
}

void AMMPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector ClosetsLocationOnSpline = Spline->FindLocationClosestToWorldLocation(
			ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
			ClosetsLocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (ClosetsLocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}
