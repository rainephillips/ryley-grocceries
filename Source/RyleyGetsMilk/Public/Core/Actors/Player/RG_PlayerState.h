// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RG_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RYLEYGETSMILK_API ARG_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
	friend class ARG_PlayerCharacter;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State", meta = (AllowPrivateAccess = true))
	bool bIsAlive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State", meta = (AllowPrivateAccess = true))
	class ARG_ItemBase* LeftHandItem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State", meta = (AllowPrivateAccess = true))
	class ARG_ItemBase* RightHandItem = nullptr;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Limb State", meta = (AllowPrivateAccess = true))
	bool bLeftLegLifted = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Limb State", meta = (AllowPrivateAccess = true))
	bool bRightLegLifted = false;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Limb State", meta = (AllowPrivateAccess = true))
	bool bLeftArmLifted = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Limb State", meta = (AllowPrivateAccess = true))
	bool bRightArmLifted = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Limb State", meta = (AllowPrivateAccess = true))
	bool bHeadDetached = false;

public:
	UFUNCTION(BlueprintCallable)
	inline bool IsAlive() const { return bIsAlive; }

	UFUNCTION(BlueprintCallable)
	inline bool IsDead() const { return !bIsAlive; }

	UFUNCTION(BlueprintCallable)
	bool HasMilk() const;

private:
	UFUNCTION()
	uint8 GetGrabbingArm() const;
};
