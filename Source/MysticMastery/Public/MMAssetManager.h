// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MMAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class MYSTICMASTERY_API UMMAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UMMAssetManager& Get();
protected:
	virtual void StartInitialLoading() override;
};
