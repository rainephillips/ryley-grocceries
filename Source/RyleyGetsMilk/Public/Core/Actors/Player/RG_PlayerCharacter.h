// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RG_PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerRespawned);


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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RespawnInvincibilityTime = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeBeforeRespawn = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* DeathSound = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	class ARG_PlayerState* GamePlayerState = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	bool bCanPlayerRespawn = true;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	bool bIsPlayerTripped = false;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	uint8 bTripStepImmunity = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TripLength = 5.f;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	float TripTimer = 0.f;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	int32 DeathCount = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Live Ragdoll Settings");
	float WaistHeight = 100.f;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintAssignable, Category = "Events")
	FOnPlayerDied OnPlayerDied;

	UPROPERTY(VisibleInstanceOnly, BlueprintAssignable, Category = "Events")
	FOnPlayerRespawned OnPlayerRespawned;

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
	
	UFUNCTION()
	void Kill();


public:
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void Look(const FVector2D& Direction);
	
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void LiftLeg(const bool bRightLeg);
	
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void DropLeg(const bool bRightLeg, const bool bForceDrop = false);
	
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void LiftArm(const bool bRightArm);
	
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void DropArm(const bool bRightArm);

	UFUNCTION(BlueprintCallable, Category = "Controls")
	void DropItem(const bool bRightArm, const bool bForceDrop = false);
	
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void SteerFeet(const FVector2D& Direction);
	
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void UpdateArmPos();
	
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void TripPlayer();
	
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void UnTripPlayer();
	
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void AddForce(const FVector& Force);

public:
	UFUNCTION(BlueprintCallable, Category = "Respawn Handling")
	void SavePlayerLocation();

	UFUNCTION(BlueprintCallable, Category = "Respawn Handling")
	void LoadPlayerLocation();
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Components", meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* Skeleton;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Components", meta = (AllowPrivateAccess = true))
	class UCameraComponent* Camera;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Components", meta = (AllowPrivateAccess = true))
	class USpringArmComponent* Boom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Components", meta = (AllowPrivateAccess = true))
	class USceneComponent* Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Components", meta = (AllowPrivateAccess = true))
	class UAudioComponent* AudioPlayer;
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Live Ragdoll Targets", meta = (AllowPrivateAccess = true))
	USceneComponent* LRT_Head;

private:
	UPROPERTY()
	FVector RightFootLiftedPos;
	
	UPROPERTY()
	FVector LeftFootLiftedPos;

	UPROPERTY()
	FVector RightFootLastPos;
	
	UPROPERTY()
	FVector LeftFootLastPos;

	UPROPERTY()
	FTimerHandle TimerHandle;
	
	UPROPERTY()
	FTimerHandle ResetLimbTimerHandle;
	
private:
	float LegLength = 0.f;
	float ArmLength = 0.f;
	bool bThighLocated = false;
	bool bShoulderLocated = false;
	
private:
	UFUNCTION()
	void GrabBone(FLiveRigTargetData& RigData);
	
	UFUNCTION()
	void AttachTargetsToBoneLocations();
	
	UFUNCTION()
	void TeleportBonesToTarget();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
		FVector NormalImpulse, const FHitResult& HitResult);
};
