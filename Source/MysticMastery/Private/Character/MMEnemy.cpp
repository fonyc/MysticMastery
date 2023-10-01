// Fill out your copyright notice in the Description page of Project Settings.


#include "MysticMastery/Public/Character/MMEnemy.h"

#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "MysticMastery/MysticMastery.h"

AMMEnemy::AMMEnemy()
{
	//In order to make enemies be able to be highlighted, set its collision response to visibility to block
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);

	//GAS
	AbilitySystemComponent = CreateDefaultSubobject<UMMAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UMMAttributeSet>("AttributeSet");
}

void AMMEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL);
	
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL);
}

void AMMEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AMMEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AMMEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMMEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

