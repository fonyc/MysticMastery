#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MMProjectile.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class MYSTICMASTERY_API AMMProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AMMProjectile();

	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	UFUNCTION()
	void OnSphereOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;
	
	bool bHit = false;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
};
