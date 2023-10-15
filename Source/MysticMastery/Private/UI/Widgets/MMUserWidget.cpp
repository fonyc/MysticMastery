// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/MMUserWidget.h"

void UMMUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
