// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actors/Player/RG_PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/Actors/Player/RG_PlayerController.h"
#include "Core/Data/Ragdoll/LiveRagdollBoneDataAsset.h"
#include "Core/Functional/Libraries/LiveRagdollHelperLib.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "Chaos/DebugDrawQueue.h"


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
	LRT_Root->AttachToComponent(Root, FAttachmentTransformRules::KeepWorldTransform);
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
	LRT_RightFoot = LiveRigTargetPoints["Head"].Target;
	
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
	LiveRigTargetPoints["Waist"].Target->SetWorldLocation(FVector{MidPoint.X, MidPoint.Y, MidPoint.Z + WaistHeight});
	LiveRigTargetPoints["Head"].Target->SetWorldLocation(FVector{MidPoint.X, MidPoint.Y, MidPoint.Z + WaistHeight * 1.25f});
	LiveRigTargetPoints["LeftHand"].Target->SetWorldLocation(FVector{LeftFootPos.X, LeftFootPos.Y, LeftFootPos.Z + WaistHeight});
	LiveRigTargetPoints["RightHand"].Target->SetWorldLocation(FVector{RightFootPos.X, RightFootPos.Y, RightFootPos.Z + WaistHeight});
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
	
	for (auto& LiveRigData : LiveRigTargetPoints)
	{
		FLiveRigTargetData& BoneData = LiveRigData.Value;
		BoneData.PhysicsHandle->SetTargetLocation(BoneData.Target->GetComponentLocation());
		
		DrawDebugBox(
	GetWorld(),                 // World context
	BoneData.Target->GetComponentLocation(),// Center location
	FVector{10.f, 10.f, 10.f},
	FColor::Red,
	false,
	0,
	10
);
	}
	
}

void ARG_PlayerCharacter::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);
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
	
	for (auto& LiveRigData : LiveRigTargetPoints)
	{
		if (FLiveRigBoneData* BoneData = LiveRigBoneData->BoneDataMap.Find(LiveRigData.Key))
		{
			FLiveRigTargetData& TargetData = LiveRigData.Value;
			TargetData.BoneTarget = BoneData->BoneName;
			TargetData.PhysicsHandle = NewObject<UPhysicsHandleComponent>(this, FName(*FString::Printf(TEXT("%s_PhysicsHandle"), *LiveRigData.Key)));
			TargetData.PhysicsHandle->RegisterComponent();
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
}

void ARG_PlayerCharacter::Look(const FVector2D& Direction)
{
	AddControllerYawInput(Direction.X);
	AddControllerPitchInput(Direction.Y);
}

void ARG_PlayerCharacter::MoveLimb(const FVector& Direction)
{
	FVector Forward = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	
	LRT_LeftFoot->AddWorldOffset(Forward * Direction);
}

