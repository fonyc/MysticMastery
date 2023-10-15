// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MMEffectActor.generated.h"

class USphereComponent;

UCLASS()
class MYSTICMASTERY_API AMMEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AMMEffectActor();

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;
};
