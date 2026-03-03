// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Shootable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UShootable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RYLEYGETSMILK_API IShootable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsShootable(class AActor* Interactor);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DealDamage(const float& Damage);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnHealthZero(class AActor* Interactor);
};
