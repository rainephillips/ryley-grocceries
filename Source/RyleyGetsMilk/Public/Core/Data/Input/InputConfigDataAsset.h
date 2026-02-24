// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "Engine/DataAsset.h"
#include "InputConfigDataAsset.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMappingContext
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputMappingContext* Context;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Priority;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModifyContextOptions Options;

};

UCLASS()
class RYLEYGETSMILK_API UInputConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Input")
	class UInputAction* Find(FName ID);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void Bind(class APlayerController* PlayerController);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TMap<FName, class UInputAction*> Actions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<FMappingContext> MappingContexts;
};
