// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Actors/Player/RG_PlayerState.h"

#include "Core/Actors/Items/RG_ItemBase.h"

bool ARG_PlayerState::HasMilk() const
{
	if (!LeftHandItem && !RightHandItem)
		return false;

	if (LeftHandItem && LeftHandItem->ActorHasTag("Milk"))
		return true;

	if (RightHandItem && RightHandItem->ActorHasTag("Milk"))
		return true;

	return false;
}

uint8 ARG_PlayerState::GetGrabbingArm() const
{
	if (!bLeftArmLifted && !bRightArmLifted)
		return 0;

	if (bLeftArmLifted && LeftHandItem == nullptr)
		return 1;

	if (bRightArmLifted && RightHandItem == nullptr)
		return 2;
	
	return 0;
}
