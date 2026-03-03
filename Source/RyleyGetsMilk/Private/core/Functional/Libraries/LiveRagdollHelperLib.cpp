// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Functional/Libraries/LiveRagdollHelperLib.h"

bool ULiveRagdollHelperLib::EnableBoneLive(class USkeletalMeshComponent* Mesh, const FName& Bone, const FLiveRagdollBoneData& BoneData)
{
	if (!Mesh)
		return false;
	
	if (!BoneData.bAffectSelf && !BoneData.bAffectChildren)
		return false;
	
	if (BoneData.bAffectChildren)
	{
		Mesh->SetAllBodiesBelowSimulatePhysics(Bone, true, BoneData.bAffectSelf);
		Mesh->SetAllBodiesBelowPhysicsBlendWeight(Bone, BoneData.Influence, true, BoneData.bAffectSelf);
	}
	else
	{
		Mesh->SetBodySimulatePhysics(Bone, true);
		if (FBodyInstance* BodyInstance = Mesh->GetBodyInstance(Bone))
			BodyInstance->PhysicsBlendWeight = BoneData.Influence;
	}
	
	
	return true;
}

bool ULiveRagdollHelperLib::DisableBoneLive(class USkeletalMeshComponent* Mesh, const FName& Bone,
	const FLiveRagdollBoneData& BoneData)
{
	if (!Mesh)
		return false;
	
	if (!BoneData.bAffectSelf && !BoneData.bAffectChildren)
		return false;
	
	if (BoneData.bAffectChildren)
		Mesh->SetAllBodiesBelowSimulatePhysics(Bone, false, BoneData.bAffectSelf);
	else
		Mesh->SetBodySimulatePhysics(Bone, false);
	
	
	return true;
}

bool ULiveRagdollHelperLib::AttachActorToSocketByName(AActor* Actor, class USkeletalMeshComponent* Mesh,
	const FName& SocketName, bool bSnapToTarget, bool bUpdateCollision, bool bCollisionEnabled)
{
	if (!IsValid(Actor) || !IsValid(Mesh))
		return false;
	
	Actor->AttachToComponent(Mesh, bSnapToTarget ? FAttachmentTransformRules::SnapToTargetNotIncludingScale : FAttachmentTransformRules::KeepRelativeTransform, SocketName);
	
	if (bUpdateCollision)
		Actor->SetActorEnableCollision(bCollisionEnabled);
	
	return true;
}

