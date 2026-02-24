// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Data/Input/InputConfigDataAsset.h"

#include "EnhancedInputSubsystems.h"

#include "Engine/LocalPlayer.h"

class UInputAction* UInputConfigDataAsset::Find(FName ID)
{
	if (Actions.Contains(ID))
	{
		return Actions.FindChecked(ID);
	}
	return nullptr;
}

void UInputConfigDataAsset::	Bind(class APlayerController* PC)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		for (auto& [Context, Priority, Options] : MappingContexts)
		{
			Subsystem->AddMappingContext(Context, Priority, Options);
		}
	}
}