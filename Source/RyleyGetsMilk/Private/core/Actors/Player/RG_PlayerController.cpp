// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actors/Player/RG_PlayerController.h"

#include "Core/Data/Input/InputConfigDataAsset.h"
#include "EnhancedInputComponent.h"
#include "Core/Actors/Player/RG_PlayerCharacter.h"
#include "Core/Functional/Libraries/CommonBlueprintFunctionLibrary.h"

ARG_PlayerController::ARG_PlayerController()
{
}

void ARG_PlayerController::BindActions(class ARG_PlayerCharacter* InPlayerCharacter)
{
	if (!IsValid(PlayerCharacter))
	{
		if (IsValid(InPlayerCharacter))
			PlayerCharacter = InPlayerCharacter;
		else
			return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (const UInputAction* Action = InputConfig->Find("Look"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, this, &ARG_PlayerController::Look);
		}

		if (const UInputAction* Action = InputConfig->Find("MoveLimb"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, this, &ARG_PlayerController::MoveLimb);
		}
		
		
		if (const UInputAction* Action = InputConfig->Find("LeftFoot"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, this, &ARG_PlayerController::MoveLeftFoot);
		}
		
		if (const UInputAction* Action = InputConfig->Find("RightFoot"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, this, &ARG_PlayerController::MoveRightFoot);
		}
	}
}

void ARG_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InputConfig))
	{
		InputConfig->Bind(this);
	}
}

void ARG_PlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);

	if (!IsValid(PlayerCharacter) && IsValid(P))
	{
		PlayerCharacter = Cast<ARG_PlayerCharacter>(P);
	}
}

void ARG_PlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	if (!Axis.IsNearlyZero() && IsValid(PlayerCharacter))
	{
		PlayerCharacter->Look(Axis);
	}
}

void ARG_PlayerController::MoveLimb(const FInputActionValue& Value)
{
	const FVector Axis = Value.Get<FVector>();

	if (!Axis.IsNearlyZero() && IsValid(PlayerCharacter))
	{
		PlayerCharacter->MoveLimb(Axis);
	}
}

void ARG_PlayerController::MoveLeftFoot(const FInputActionValue& Value)
{
	
	FVector MousePos;
	if (UCommonBlueprintFunctionLibrary::GetMousePosInWorldCoordinates(this, MousePos))
		PlayerCharacter->MoveLeftFoot(MousePos);
}

void ARG_PlayerController::MoveRightFoot(const FInputActionValue& Value)
{
	FVector MousePos;
	if (UCommonBlueprintFunctionLibrary::GetMousePosInWorldCoordinates(this, MousePos))
		PlayerCharacter->MoveRightFoot(MousePos);
}
