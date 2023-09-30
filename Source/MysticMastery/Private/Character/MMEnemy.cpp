// Fill out your copyright notice in the Description page of Project Settings.


#include "MysticMastery/Public/Character/MMEnemy.h"

#include "MysticMastery/MysticMastery.h"

AMMEnemy::AMMEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
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

