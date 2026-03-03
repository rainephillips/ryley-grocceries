// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Core/Types/LiveRagdollBoneData.h"
#include "LiveRagdollHelperLib.generated.h"

/**
 * 
 */
UCLASS()
class RYLEYGETSMILK_API ULiveRagdollHelperLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	static bool EnableBoneLive(class USkeletalMeshComponent* Mesh, const FName& Bone, const FLiveRagdollBoneData& BoneData);
	
	UFUNCTION(BlueprintCallable)
	static bool DisableBoneLive(class USkeletalMeshComponent* Mesh, const FName& Bone, const FLiveRagdollBoneData& BoneData);
	
	UFUNCTION(BlueprintCallable)
	static bool AttachActorToSocketByName(AActor* Actor, class USkeletalMeshComponent* Mesh, const FName& SocketName,
		bool bSnapToTarget = true,
		bool bUpdateCollision = false, bool bCollisionEnabled = false);
	
	
};
