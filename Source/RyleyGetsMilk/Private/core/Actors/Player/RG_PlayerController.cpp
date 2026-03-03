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

		if (const UInputAction* Action = InputConfig->Find("SteerFeet"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, this, &ARG_PlayerController::SteerFeet);
		}
		
		
		if (const UInputAction* Action = InputConfig->Find("Die"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, PlayerCharacter, &ARG_PlayerCharacter::Kill);
		}
		
		
		if (const UInputAction* Action = InputConfig->Find("LeftFoot"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, PlayerCharacter, &ARG_PlayerCharacter::LiftLeg, false);
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Canceled, PlayerCharacter, &ARG_PlayerCharacter::DropLeg, false);
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Completed, PlayerCharacter, &ARG_PlayerCharacter::DropLeg, false);
		}
		
		if (const UInputAction* Action = InputConfig->Find("RightFoot"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, PlayerCharacter, &ARG_PlayerCharacter::LiftLeg, true);
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Canceled, PlayerCharacter, &ARG_PlayerCharacter::DropLeg, true);
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Completed, PlayerCharacter, &ARG_PlayerCharacter::DropLeg, true);
		}
		
		if (const UInputAction* Action = InputConfig->Find("LeftArm"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, PlayerCharacter, &ARG_PlayerCharacter::LiftArm, false);
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Canceled, PlayerCharacter, &ARG_PlayerCharacter::DropArm, false);
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Completed, PlayerCharacter, &ARG_PlayerCharacter::DropArm, false);
		}
		
		if (const UInputAction* Action = InputConfig->Find("RightArm"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, PlayerCharacter, &ARG_PlayerCharacter::LiftArm, true);
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Canceled, PlayerCharacter, &ARG_PlayerCharacter::DropArm, true);
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Completed, PlayerCharacter, &ARG_PlayerCharacter::DropArm, true);
		}

		if (const UInputAction* Action = InputConfig->Find("LeftDrop"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, PlayerCharacter, &ARG_PlayerCharacter::DropItem, false);
		}
		
		if (const UInputAction* Action = InputConfig->Find("RightDrop"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, PlayerCharacter, &ARG_PlayerCharacter::DropItem, true);
		}

		if (const UInputAction* Action = InputConfig->Find("DebugSave"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, PlayerCharacter, &ARG_PlayerCharacter::SavePlayerLocation);
		}

		if (const UInputAction* Action = InputConfig->Find("DebugLoad"))
		{
			EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, PlayerCharacter, &ARG_PlayerCharacter::LoadPlayerLocation);
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

void ARG_PlayerController::SteerFeet(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	
	PlayerCharacter->SteerFeet(Axis);
}
