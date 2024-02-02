// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/MMCharacter.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/MMPlayerController.h"
#include "Player/MMPlayerState.h"
#include "UI/HUD/MMHUD.h"


// Sets default values
AMMCharacter::AMMCharacter()
{
	//Rotate character to face movement direction
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Level up Particle system
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);

	SpringArmComponent->TargetArmLength = 750.0f;
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->SetUsingAbsoluteRotation(true);

	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bInheritYaw = false;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	CharacterClass = ECharacterClass::Mage;
}

int32 AMMCharacter::GetPlayerLevel_Implementation() const 
{
	const AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);
	return MMPlayerState->GetPlayerLevel();
}

void AMMCharacter::AddXP_Implementation(int32 DeltaXP)
{
	AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);
	MMPlayerState->AddXP(DeltaXP);
}

void AMMCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		//Orient the VFX towards the camera
		const FVector CameraLocation = Camera->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator CameraNiagaraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(CameraNiagaraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}

void AMMCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

int32 AMMCharacter::GetXP_Implementation() const
{
	const AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);
	return MMPlayerState->GetXP();
}

int32 AMMCharacter::FindLevelByXP_Implementation(int32 XP)
{
	const AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);
	return MMPlayerState->LevelUpInfo->GetLevelByExperience(XP);
}

int32 AMMCharacter::GetAttributePointsReward_Implementation(const int32 InLevel) const
{
	const AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);
	return MMPlayerState->LevelUpInfo->GetAttributePointsRewardByLevel(InLevel);
}

int32 AMMCharacter::GetSpellPointsReward_Implementation(const int32 InLevel) const
{
	const AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);
	return MMPlayerState->LevelUpInfo->GetSpellPointsRewardByLevel(InLevel);
}

void AMMCharacter::AddAttributePoints_Implementation(int32 InAttributePoints)
{
	AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);
	//TODO: Add Attribute points to player state
}

void AMMCharacter::AddSpellPoints_Implementation(int32 InSpellPoints)
{
	AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);

	//TODO: Add spell points to player state
}

void AMMCharacter::AddPlayerLevel_Implementation(int32 InLevelsToAdd)
{
	AMMPlayerState* MMPlayerState = GetPlayerState<AMMPlayerState>();
	check(MMPlayerState);
	MMPlayerState->AddLevel(InLevelsToAdd);
}

void AMMCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Init ability actor info for the SERVER 
	InitializeAbilityActorInfo();
	
	//These function adds normal and passive abilities
	AddCharacterAbilities();
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

