// Fill out your copyright notice in the Description page of Project Settings.

#include "MysticMastery/Public/Character/MMEnemy.h"
#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MMAbilitySystemComponent.h"
#include "AbilitySystem/MMAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "MysticMastery/MysticMastery.h"
#include "UI/Widgets/MMUserWidget.h"
#include "MMGameplayTags.h"
#include "AI/MMAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AMMEnemy::AMMEnemy()
{
	//In order to make enemies be able to be highlighted, set its collision response to visibility to block
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	//GAS
	AbilitySystemComponent = CreateDefaultSubobject<UMMAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UMMAttributeSet>("AttributeSet");

	//Rotation settings
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
	//Healthbar component
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

/**
 * Method that, when the controller posses the actor, initialize its controller (custom AI controller in this case)
 * And give it BB and BT...
 */
void AMMEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority()) return;
	MMAIController = Cast<AMMAIController>(NewController);

	MMAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviourTree->BlackboardAsset);
	MMAIController->RunBehaviorTree(BehaviourTree);
	MMAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	MMAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangeAttack"), CharacterClass != ECharacterClass::Warrior);

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

void AMMEnemy::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}

void AMMEnemy::BeginPlay()
{
	Super::BeginPlay();

	//Set initial speed
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting? 0 : BaseWalkSpeed;
	InitializeAbilityActorInfo();
	
	if(HasAuthority())
	{
		UMMAbilitySystemBlueprintLibrary::GiveStartupAbilities(this,AbilitySystemComponent, CharacterClass);
	}

	//Set this very class as the widget controller on health bar
	if (UMMUserWidget* MMUserWidget = Cast<UMMUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		MMUserWidget->SetWidgetController(this);
	}

	//Bind lambdas to our callbacks 
	if (const UMMAttributeSet* MMAS = Cast<UMMAttributeSet>(AttributeSet))
	{
		//Listen to changes on Health or MaxHealth to draw the Health bar component on the enemy
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
		
		//Register for a gameplay tag change so enemy can react to damage
		FGameplayTag Tag = FMMGameplayTags::Get().Effects_HitReact;
		AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddUObject(this,&AMMEnemy::HitReactTagChanged);

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

	if(HasAuthority())
	{
		InitializeDefaultAttributes();
	}
}

void AMMEnemy::InitializeDefaultAttributes() const
{
	UMMAbilitySystemBlueprintLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AMMEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewTagCount)
{
	bHitReacting = NewTagCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting? 0 : BaseWalkSpeed;
	MMAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
}

void AMMEnemy::MulticastHandleDeath_Implementation()
{
	Super::MulticastHandleDeath_Implementation();
	
	FDetachmentTransformRules DetachmentTransformRules = FDetachmentTransformRules::KeepWorldTransform;
	HealthBar->DetachFromComponent(DetachmentTransformRules);
}

void AMMEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMMEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
