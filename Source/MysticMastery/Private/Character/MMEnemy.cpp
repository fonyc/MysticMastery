// Fill out your copyright notice in the Description page of Project Settings.


#include "MysticMastery/Public/Character/MMEnemy.h"


// Sets default values
AMMEnemy::AMMEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMMEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMMEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMMEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

