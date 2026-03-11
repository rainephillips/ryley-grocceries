// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RG_PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class RYLEYGETSMILK_API ARG_PlayerHUD : public AHUD
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY()
	class UUserWidget* HUDWidgetInstance;

protected:
	virtual void BeginPlay() override;
};
