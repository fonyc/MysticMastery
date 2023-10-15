// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MMUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 
	* Responsible of setting the WidgetController and call the WidgetControllerSet afterwards.   
	*/
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	/** 
	* Called immediately after setting the Widget Controller. Responsible for setting Widget data.  
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
