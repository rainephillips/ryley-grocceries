// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actors/Player/RG_PlayerHUD.h"

#include "Blueprint/UserWidget.h"

void ARG_PlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HUDWidgetClass))
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(GetOwningPlayerController(), HUDWidgetClass);
		HUDWidgetInstance->AddToPlayerScreen();
	}
}
