// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RG_PlayerCharacter.generated.h"

USTRUCT(BlueprintType)
struct FLiveRigTargetData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* Target = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UPhysicsHandleComponent* PhysicsHandle = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneTarget = NAME_None;
};

UCLASS()
class RYLEYGETSMILK_API ARG_PlayerCharacter : public APawn
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class ULiveRagdollBoneDataAsset* LiveRigBoneData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live Ragdoll Settings");
	float WaistHeight = 100.f;

public:
	// Sets default values for this character's properties
	ARG_PlayerCharacter();
	
public:
	UFUNCTION(BlueprintCallable)
	void MoveLeftFoot(const FVector& NewPosition);
	
	UFUNCTION(BlueprintCallable)
	void MoveRightFoot(const FVector& NewPosition);
	
	UFUNCTION(BlueprintCallable)
	void UpdateTargetPositions();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	UFUNCTION()
	void StartRagdoll();


public:
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void Move(const FVector2D& Direction);

	UFUNCTION(BlueprintCallable, Category = "Controls")
	void Look(const FVector2D& Direction);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bIsRagdolling = false;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Components", meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* Skeleton;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Components", meta = (AllowPrivateAccess = true))
	class UCameraComponent* Camera;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Components", meta = (AllowPrivateAccess = true))
	class USpringArmComponent* Boom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Components", meta = (AllowPrivateAccess = true))
	class USceneComponent* Root;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Live Ragdoll Targets", meta = (AllowPrivateAccess = true))
	TMap<FString, FLiveRigTargetData> LiveRigTargetPoints;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Live Ragdoll Targets", meta = (AllowPrivateAccess = true))
	USceneComponent* LRT_Root;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Live Ragdoll Targets", meta = (AllowPrivateAccess = true))
	USceneComponent* LRT_Waist;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Live Ragdoll Targets", meta = (AllowPrivateAccess = true))
	USceneComponent* LRT_LeftHand;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Live Ragdoll Targets", meta = (AllowPrivateAccess = true))
	USceneComponent* LRT_RightHand;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Live Ragdoll Targets", meta = (AllowPrivateAccess = true))
	USceneComponent* LRT_LeftFoot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Live Ragdoll Targets", meta = (AllowPrivateAccess = true))
	USceneComponent* LRT_RightFoot;
};
