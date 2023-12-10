// Fill out your copyright notice in the Description page of Project Settings.

#include "MysticMastery/Public/Character/MMEnemy.h"
#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "MysticMastery/MysticMastery.h"
#include "UI/Widgets/MMUserWidget.h"

AMMEnemy::AMMEnemy()
{
	//In order to make enemies be able to be highlighted, set its collision response to visibility to block
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	//GAS
	AbilitySystemComponent = CreateDefaultSubobject<UMMAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UMMAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
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
	InitializeAbilityActorInfo();

	//Set this very class as the widget controller on health bar
	if (UMMUserWidget* MMUserWidget = Cast<UMMUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		MMUserWidget->SetWidgetController(this);
	}

	//Bind lambdas to our callbacks
	if (const UMMAttributeSet* MMAS = Cast<UMMAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MMAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});

		//Broadcast initial values
		OnHealthChanged.Broadcast(MMAS->GetHealth());
		OnMaxHealthChanged.Broadcast(MMAS->GetMaxHealth());
	}
}

void AMMEnemy::InitializeAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	//Warn the delegate we just set the ability actor info
	Cast<UMMAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	InitializeDefaultAttributes();
}

void AMMEnemy::InitializeDefaultAttributes() const
{
	UMMAbilitySystemBlueprintLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AMMEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMMEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
