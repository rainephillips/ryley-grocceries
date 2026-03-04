// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RG_PlayerState.generated.h"

USTRUCT(BlueprintType)
struct FPlayerRespawnData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LeftFootLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RightFootLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HeadLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WaistLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MainLocation;
};

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Live Ragdoll Targets", meta = (AllowPrivateAccess = true))
	bool bIsInvincible = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Live Ragdoll Targets", meta = (AllowPrivateAccess = true))
	FPlayerRespawnData RespawnData;

public:
	UFUNCTION(BlueprintCallable)
	inline bool IsAlive() const { return bIsAlive; }

	UFUNCTION(BlueprintCallable)
	inline bool IsDead() const { return !bIsAlive; }

	UFUNCTION(BlueprintCallable)
	inline bool IsInvincible() const { return bIsInvincible; }

	UFUNCTION(BlueprintCallable)
	inline void RevertInvincibility() { bIsInvincible = false; }

	UFUNCTION(BlueprintCallable)
	bool HasMilk() const;

private:
	UFUNCTION()
	uint8 GetGrabbingArm() const;
};
