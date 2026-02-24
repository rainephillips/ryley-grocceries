// Fill out your copyright notice in the Description page of Project Settings.


#include "core/gameplay/gamemodes/RG_GameMode.h"

#include "Core/Functional/Libraries/CommonBlueprintFunctionLibrary.h"

void ARG_GameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UCommonBlueprintFunctionLibrary::SetGameAndUIInputMode(this, EMouseLockMode::DoNotLock, nullptr, 0);
}
