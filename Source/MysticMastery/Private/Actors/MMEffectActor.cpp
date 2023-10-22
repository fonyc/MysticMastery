// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MMEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "Components/SphereComponent.h"

AMMEffectActor::AMMEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	SetRootComponent(MeshComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetupAttachment(RootComponent);
}

//TODO: Remove this disgusting mutable thing for a Gameplay Effect
void AMMEffectActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Warning,TEXT("Begin Overlap"));

	if(const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		const UMMAttributeSet* AttributeSet = Cast<UMMAttributeSet>(AbilitySystemInterface->GetAbilitySystemComponent()->GetAttributeSet(UMMAttributeSet::StaticClass()));
		UMMAttributeSet* MutableAttributeSet = const_cast<UMMAttributeSet*>(AttributeSet);
		MutableAttributeSet->SetHealth(AttributeSet->GetHealth() + 25.f);
		MutableAttributeSet->SetMana(AttributeSet->GetMana() + 25.f);
		Destroy();
	}
	
}

void AMMEffectActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp,Warning,TEXT("End Overlap"));
}


// Called when the game starts or when spawned
void AMMEffectActor::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AMMEffectActor::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AMMEffectActor::OnEndOverlap);
}
