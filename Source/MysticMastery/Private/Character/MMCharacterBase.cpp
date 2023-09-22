// Fill out your copyright notice in the Description page of Project Settings.


#include "MysticMastery/Public/Character/MMCharacterBase.h"


// Sets default values
AMMCharacterBase::AMMCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	
	//Remove any collision from weapon
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}
