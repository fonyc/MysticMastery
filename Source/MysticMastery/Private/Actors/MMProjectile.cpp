#include "Actors/MMProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"
#include "MysticMastery/MysticMastery.h"

AMMProjectile::AMMProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SetRootComponent(SphereComponent);

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionObjectType(ECC_Projectile);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMMProjectile::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AMMProjectile::OnSphereOverlap);
	
	SetLifeSpan(LifeSpan);
	UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent(), NAME_None, FVector(ForceInit), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);

	//Ignore Instigator of the projectile
	if (AActor* MyInstigator = GetInstigator(); IsValid(MyInstigator))
	{
		SphereComponent->IgnoreActorWhenMoving(MyInstigator, true);
	}
}

void AMMProjectile::Destroyed()
{
	//(If the client was first, its flag was raised so the effects wont play twice. If the client was after the
	//server, then this method ensures the effects are played at least once)
	if (!bHit && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	}
	
	Super::Destroyed();
}

void AMMProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Check data is not null and the effect caused do not hit himself
	if (DamageEffectSpecHandle.Data.IsValid() && DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor)
	{
		return;
	}

	//Check that the arrow is not against a friendly target
	if (UMMAbilitySystemBlueprintLibrary::IsFriendlyActor(DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser(), OtherActor))
	{
		return;
	}
		
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

	//In the (very unlikely) case that the destruction happens before the client has had its sphere overlapped
	//The destruction is made by the server. If the clients comes here first, just raises a flag (and plays the effects)
	if (HasAuthority())
	{
		//Get the OtherACtor ASC and apply the GE to it
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		
		Destroy();
	}
	else
	{
		bHit = true;
	}
}

void AMMProjectile::DisableMeshIfAny(UStaticMeshComponent* StaticMesh)
{
	if (bDisableMeshCollision && StaticMesh != nullptr && IsValid(StaticMesh)) StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);	
}
