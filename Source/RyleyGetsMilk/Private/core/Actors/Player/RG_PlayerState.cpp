// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Actors/Player/RG_PlayerState.h"

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
