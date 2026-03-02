// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actors/Player/RG_PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Core/Actors/Player/RG_PlayerController.h"
#include "Core/Data/Ragdoll/LiveRagdollBoneDataAsset.h"
#include "Core/Functional/Libraries/LiveRagdollHelperLib.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "Core/Functional/Libraries/CommonBlueprintFunctionLibrary.h"

#define LEG_LENGTH_MULT 1.25f
#define ARM_LENGTH_MULT 1.5f
#define HANDLE_INTERP 10.0f

// Sets default values
ARG_PlayerCharacter::ARG_PlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAsyncPhysicsTickEnabled = true;

	Root = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(Root);
	
	Skeleton = CreateDefaultSubobject<USkeletalMeshComponent>("Skeleton");
	Skeleton->SetupAttachment(Root);

	Skeleton->SetMassOverrideInKg(NAME_None, 10.0f, true);
	
	Skeleton->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	
	LRT_Root = CreateDefaultSubobject<USceneComponent>("LRT_Root");
	LRT_Root->SetupAttachment(Root);
	LRT_Root->SetRelativeLocationAndRotation(FVector{0.f, 0.f, -90.f}, FRotator{0.f, -90.f, 0.f});
	
	const TArray<FString> BoneNames = {"Waist", "LeftHand", "RightHand", "LeftFoot", "RightFoot", "Head"};
	for (const FString& Bone : BoneNames)
	{
		USceneComponent* BoneTarget = CreateDefaultSubobject<USceneComponent>(
			FName(*FString::Printf(TEXT("LRG_%s"), *Bone))
			);
		BoneTarget->SetupAttachment(LRT_Root);
		LiveRigTargetPoints.Add(Bone, FLiveRigTargetData{BoneTarget, nullptr, NAME_None});
	}
	
	LRT_Waist = LiveRigTargetPoints["Waist"].Target;
	LRT_LeftHand = LiveRigTargetPoints["LeftHand"].Target;
	LRT_RightHand = LiveRigTargetPoints["RightHand"].Target;
	LRT_LeftFoot = LiveRigTargetPoints["LeftFoot"].Target;
	LRT_RightFoot = LiveRigTargetPoints["RightFoot"].Target;
	LRT_Head = LiveRigTargetPoints["Head"].Target;
	
	Boom = CreateDefaultSubobject<USpringArmComponent>("Boom");
	Boom->SetupAttachment(LRT_Waist);
	Boom->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	Boom->TargetArmLength = 400.0f;
	Boom->bUsePawnControlRotation = true;
	Boom->bEnableCameraLag = true;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(Boom);

}

void ARG_PlayerCharacter::MoveLeftFoot(const FVector& NewPosition)
{
	LiveRigTargetPoints["LeftFoot"].Target->SetWorldLocation(NewPosition);
	UpdateTargetPositions();
}

void ARG_PlayerCharacter::MoveRightFoot(const FVector& NewPosition)
{
	LiveRigTargetPoints["RightFoot"].Target->SetWorldLocation(NewPosition);
	UpdateTargetPositions();
}

void ARG_PlayerCharacter::UpdateTargetPositions()
{
	const FVector LeftFootPos = LiveRigTargetPoints["LeftFoot"].Target->GetComponentLocation();
	const FVector RightFootPos = LiveRigTargetPoints["RightFoot"].Target->GetComponentLocation();
	
	const FVector MidPoint = (LeftFootPos + RightFootPos) * 0.5f;
	LiveRigTargetPoints["Waist"].Target->SetWorldLocation(FVector{MidPoint.X, MidPoint.Y, std::min(LeftFootPos.Z, RightFootPos.Z) + WaistHeight});
	LiveRigTargetPoints["Head"].Target->SetWorldLocation(FVector{MidPoint.X, MidPoint.Y, std::min(LeftFootPos.Z, RightFootPos.Z) + WaistHeight * 1.25f});
}

// Called when the game starts or when spawned
void ARG_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	StartRagdoll();
	
}

// Called every frame
void ARG_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateArmPos();
	
}

void ARG_PlayerCharacter::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);
	
	for (auto& LiveRigData : LiveRigTargetPoints)
	{
		FLiveRigTargetData& BoneData = LiveRigData.Value;
		BoneData.PhysicsHandle->SetTargetLocation(BoneData.Target->GetComponentLocation());
		
		DrawDebugSphere(
	GetWorld(),                 // World context
	BoneData.Target->GetComponentLocation(),// Center location
10.f, 2, FColor::Green, false, 0.f
);
	}
}

// Called to bind functionality to input
void ARG_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (ARG_PlayerController* PC = Cast<ARG_PlayerController>(Controller))
	{
		PC->BindActions(this);
	}
}

void ARG_PlayerCharacter::StartRagdoll()
{
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Skeleton->SetCollisionObjectType(ECC_Pawn);
	Skeleton->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	
	ULiveRagdollHelperLib::EnableBoneLive(Skeleton, "Waist", FLiveRagdollBoneData{ 1.f, true, true});
	bIsRagdolling = true;

	TArray<FString> KeysToRemove;
	
	// Create physics handles
	for (auto& LiveRigData : LiveRigTargetPoints)
	{
		if (FLiveRigBoneData* BoneData = LiveRigBoneData->BoneDataMap.Find(LiveRigData.Key))
		{
			FLiveRigTargetData& TargetData = LiveRigData.Value;
			TargetData.BoneTarget = BoneData->BoneName;
			TargetData.PhysicsHandle = NewObject<UPhysicsHandleComponent>(this, FName(*FString::Printf(TEXT("%s_PhysicsHandle"), *LiveRigData.Key)));
			TargetData.PhysicsHandle->RegisterComponent();
			TargetData.PhysicsHandle->InterpolationSpeed = HANDLE_INTERP;
			FVector BonePos = Skeleton->GetBoneLocation(TargetData.BoneTarget, EBoneSpaces::WorldSpace);
			UE_LOG(LogTemp, Warning, TEXT("Grabbing %s | Simulating: %d"),
	*TargetData.BoneTarget.ToString(),
	Skeleton->IsSimulatingPhysics(TargetData.BoneTarget));
			TargetData.PhysicsHandle->GrabComponentAtLocation(Skeleton, TargetData.BoneTarget, BonePos);
		}
		else
			KeysToRemove.Add(LiveRigData.Key);
	}
	
	for (const FString& Key : KeysToRemove)
		LiveRigTargetPoints.Remove(Key);
	
	LiveRigTargetPoints["LeftHand"].PhysicsHandle->ReleaseComponent();
	LiveRigTargetPoints["RightHand"].PhysicsHandle->ReleaseComponent();
	
	// Find feet and thigh bones
	
	if ( const FLiveRigBoneData* Thigh = LiveRigBoneData->BoneDataMap.Find("LeftThigh"))
		if ( const FLiveRigBoneData* Foot = LiveRigBoneData->BoneDataMap.Find("LeftFoot"))
		{
			// freaky ass code
			const FVector ThighPos = Skeleton->GetBoneLocation(Thigh->BoneName, EBoneSpaces::WorldSpace);
			const FVector FeetPos = Skeleton->GetBoneLocation(Foot->BoneName, EBoneSpaces::WorldSpace);
			
			LegLength = UKismetMathLibrary::Vector_Distance(ThighPos, FeetPos) * LEG_LENGTH_MULT;
			bThighLocated = true;
		}
	
	
	// Find Shoulder and Hand Bones
	if ( const FLiveRigBoneData* Shoulder = LiveRigBoneData->BoneDataMap.Find("LeftShoulder"))
		if ( const FLiveRigBoneData* Hand = LiveRigBoneData->BoneDataMap.Find("LeftHand"))
		{
			// freaky ass code
			const FVector ShoulderPos = Skeleton->GetBoneLocation(Shoulder->BoneName, EBoneSpaces::WorldSpace);
			const FVector HandPos = Skeleton->GetBoneLocation(Hand->BoneName, EBoneSpaces::WorldSpace);
			
			ArmLength = UKismetMathLibrary::Vector_Distance(ShoulderPos, HandPos) * ARM_LENGTH_MULT;
			bShoulderLocated = true;
		}
}

void ARG_PlayerCharacter::Kill()
{
	for (auto& LiveRigData : LiveRigTargetPoints)
	{
		FLiveRigTargetData& BoneData = LiveRigData.Value;
		BoneData.PhysicsHandle->ReleaseComponent();
	}
}

void ARG_PlayerCharacter::Look(const FVector2D& Direction)
{
	AddControllerYawInput(Direction.X);
	AddControllerPitchInput(Direction.Y);
}

void ARG_PlayerCharacter::MoveLimb(const FVector& Direction)
{
	FVector Forward = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	
	LRT_LeftFoot->AddWorldOffset(Forward);
}

void ARG_PlayerCharacter::LiftLeg(const bool bRightLeg)
{
	if (bThighLocated)
	{
		FVector ThighPos = Skeleton->GetBoneLocation(LiveRigBoneData->BoneDataMap[(bRightLeg) ? "RightThigh" : "LeftThigh"].BoneName, EBoneSpaces::WorldSpace);
		FVector EndPos = ThighPos + FVector(0.0f, 0.f, LegLength);
		LiveRigTargetPoints[(bRightLeg) ? "RightFoot" : "LeftFoot"].Target->SetWorldLocation(EndPos);
		
		if (bRightLeg)
		{
			RightFootLiftedPos = EndPos;
			bRightLegLifted = true;
		}
		else
		{
			LeftFootLiftedPos = EndPos;
			bLeftLegLifted = true;
		}
	}
}

void ARG_PlayerCharacter::SteerFeet(const FVector2D& Direction)
{
	const FVector Forward = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	const FVector Right   = UKismetMathLibrary::GetRightVector(GetControlRotation());

	FVector TranslatedDirection =
		(Direction.X * Right) +
		(Direction.Y * Forward);

	TranslatedDirection.Z = 0.f;
	TranslatedDirection.Normalize();
	
	if (bLeftLegLifted)
	{
		LiveRigTargetPoints["LeftFoot"].Target->SetWorldLocation
		(LeftFootLiftedPos + TranslatedDirection * LegLength);
	}
	
	if (bRightLegLifted)
	{
		LiveRigTargetPoints["RightFoot"].Target->SetWorldLocation
		(RightFootLiftedPos + TranslatedDirection * LegLength);
	}
}

void ARG_PlayerCharacter::UpdateArmPos()
{
	if (!bShoulderLocated)
		return;
	
	FVector Forward = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	
	if (bLeftArmLifted)
	{
		FVector ShoulderPos = Skeleton->GetBoneLocation(LiveRigBoneData->BoneDataMap["LeftShoulder"].BoneName, EBoneSpaces::WorldSpace);
		LiveRigTargetPoints["LeftHand"].Target->SetWorldLocation(ShoulderPos + Forward * ArmLength);
	}
	
	if (bRightArmLifted)
	{
		FVector ShoulderPos = Skeleton->GetBoneLocation(LiveRigBoneData->BoneDataMap["RightShoulder"].BoneName, EBoneSpaces::WorldSpace);
		LiveRigTargetPoints["RightHand"].Target->SetWorldLocation(ShoulderPos + Forward * ArmLength);
	}
}

void ARG_PlayerCharacter::GrabBone(FLiveRigTargetData& RigData)
{
	FVector BoneLocation = Skeleton->GetBoneLocation(RigData.BoneTarget, EBoneSpaces::WorldSpace);
	RigData.PhysicsHandle->GrabComponentAtLocation(Skeleton, RigData.BoneTarget, BoneLocation);
}

void ARG_PlayerCharacter::DropLeg(const bool bRightLeg)
{
	FVector StartingPos = LiveRigTargetPoints[(bRightLeg) ? "RightFoot" : "LeftFoot"].Target->GetComponentLocation();
	
	bool bDidHit;
	FVector EndPos = UCommonBlueprintFunctionLibrary::GetFirstHitLocation(this, StartingPos, FVector{0.f, 0.f, -1.f}, 
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn), bDidHit, TArray<AActor*>{this});
	
	
	if (bDidHit)
	{
		LiveRigTargetPoints[(bRightLeg) ? "RightFoot" : "LeftFoot"].Target->SetWorldLocation(EndPos);
		UpdateTargetPositions();
	}
	
	if (bRightLeg)
		bRightLegLifted = false;
	else
		bLeftLegLifted = false;
}

void ARG_PlayerCharacter::LiftArm(const bool bRightArm)
{
	GrabBone(LiveRigTargetPoints[(bRightArm) ? "RightHand" : "LeftHand"]);
	
	if (bRightArm)
		bRightArmLifted = true;
	else
		bLeftArmLifted = true;
	
}

void ARG_PlayerCharacter::DropArm(const bool bRightArm)
{
	LiveRigTargetPoints[(bRightArm) ? "RightHand" : "LeftHand"].PhysicsHandle->ReleaseComponent();
	
	if (bRightArm)
		bRightArmLifted = false;
	else
		bLeftArmLifted = false;
}


