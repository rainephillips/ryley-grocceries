// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LiveRagdollBoneDataAsset.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FLiveRigBoneData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneName;
	
};

UCLASS()
class RYLEYGETSMILK_API ULiveRagdollBoneDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, FLiveRigBoneData> BoneDataMap;
	
};
