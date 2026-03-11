// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "RG_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RYLEYGETSMILK_API ARG_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ARG_PlayerController();
	
public:
	UFUNCTION(BlueprintCallable)
	void BindActions(class ARG_PlayerCharacter* InPlayerCharacter);
	
protected:
	virtual void BeginPlay() override;
	virtual void AcknowledgePossession(class APawn* P) override;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UInputConfigDataAsset* InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class ARG_PlayerCharacter* PlayerCharacter;
	
private:
	
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	
	UFUNCTION()
	void SteerFeet(const FInputActionValue& Value);
	
	UFUNCTION()
	void QuitGame();
};
