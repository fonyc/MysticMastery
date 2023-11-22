
#include "AbilitySystem/MMAbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MMPlayerState.h"
#include "UI/HUD/MMHUD.h"
#include "UI/WidgetController/MMWidgetController.h"

UOverlayWidgetController* UMMAbilitySystemBlueprintLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AMMHUD* MMHUD = Cast<AMMHUD>(PC->GetHUD()))
		{
			AMMPlayerState* PS = PC->GetPlayerState<AMMPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return MMHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UMMAbilitySystemBlueprintLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AMMHUD* MMHUD = Cast<AMMHUD>(PC->GetHUD()))
		{
			AMMPlayerState* PS = PC->GetPlayerState<AMMPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return MMHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}
