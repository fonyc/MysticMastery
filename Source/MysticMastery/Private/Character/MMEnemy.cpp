// Fill out your copyright notice in the Description page of Project Settings.


#include "MysticMastery/Public/Character/MMEnemy.h"

AMMEnemy::AMMEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMMEnemy::HighlightActor()
{
}

void AMMEnemy::UnHighlightActor()
{
	
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

