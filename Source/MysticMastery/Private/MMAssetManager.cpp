// Fill out your copyright notice in the Description page of Project Settings.


#include "MMAssetManager.h"
#include "MMGameplayTags.h"

UMMAssetManager& UMMAssetManager::Get()
{
	check(GEngine);
	return *Cast<UMMAssetManager>(GEngine->AssetManager);
}

void UMMAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//Load native tags
	FMMGameplayTags::InitializeNativeGameplayTags();
}
