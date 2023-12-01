#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MMProjectile.generated.h"

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
	
	UFUNCTION()
	void OnSphereOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;
};
