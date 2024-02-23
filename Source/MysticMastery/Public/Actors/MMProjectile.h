#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MMAbilitySystemTypes.h"
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
	
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY()
	TObjectPtr<USceneComponent> HomingSceneReference;

protected:
	virtual void BeginPlay() override;
	
	void OnHitActions();
	
	virtual void Destroyed() override;
	
	UFUNCTION()
	void OnSphereOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:

	UFUNCTION(BlueprintCallable)
	void DisableMeshIfAny(UStaticMeshComponent* MeshComponent);

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess = true))
	bool bDisableMeshCollision = true;

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 10.f;
	
	bool bHit = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;
};
