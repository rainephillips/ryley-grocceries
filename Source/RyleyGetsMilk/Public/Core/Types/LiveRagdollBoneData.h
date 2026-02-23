// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LiveRagdollBoneData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FLiveRagdollBoneData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Influence = 1.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAffectChildren = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAffectSelf = false;
};

USTRUCT(BlueprintType)
struct FLiveRagdollSkeletonData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, FLiveRagdollBoneData> BoneData;
};