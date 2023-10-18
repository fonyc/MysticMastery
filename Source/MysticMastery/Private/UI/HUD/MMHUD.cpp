// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/MMHUD.h"

#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/Widgets/MMUserWidget.h"

UOverlayWidgetController* AMMHUD::GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
	}
	return OverlayWidgetController;
}

//In charge of construct the widget controller, the widget, set the Widget Controller and add it to the viewport
void AMMHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class is not initialized. Fill out BP_MMHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class is not initialized. Fill out BP_MMHUD"));

	UUserWidget* UserWidget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UMMUserWidget>(UserWidget);

	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	OverlayWidget->SetWidgetController(WidgetController);

	UserWidget->AddToViewport();
}
