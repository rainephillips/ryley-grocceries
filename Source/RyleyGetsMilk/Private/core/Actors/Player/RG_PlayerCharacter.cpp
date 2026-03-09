// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actors/Player/RG_PlayerCharacter.h"

#include "Camera/CameraComponent.h"

#include "Components/AudioComponent.h"

#include "Core/Actors/Items/RG_ItemBase.h"
#include "Core/Actors/Player/RG_PlayerController.h"
#include "Core/Actors/Player/RG_PlayerState.h"
#include "Core/Data/Ragdoll/LiveRagdollBoneDataAsset.h"
#include "Core/Functional/Libraries/LiveRagdollHelperLib.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "Core/Functional/Libraries/CommonBlueprintFunctionLibrary.h"

#define LEG_LENGTH_HEIGHT_MULT 0.75f
#define LEG_LENGTH_DIST_MULT 2.f
#define ARM_LENGTH_MULT 1.5f
#define FUNNY_RAGDOLL_MULT 10.f
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

	Skeleton->SetCollisionProfileName(FName("Pawn"));
	Skeleton->SetNotifyRigidBodyCollision(true);
	Skeleton->SetAllBodiesNotifyRigidBodyCollision(true);
	Skeleton->SetGenerateOverlapEvents(true);
	Skeleton->OnComponentHit.AddDynamic(this, &ARG_PlayerCharacter::OnHit);

	AudioPlayer = CreateDefaultSubobject<UAudioComponent>("AudioPlayer");
	AudioPlayer->SetupAttachment(Root);
	AudioPlayer->bCanPlayMultipleInstances = true;

	Tags.Add("Player");

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
	LiveRigTargetPoints["Head"].Target->SetWorldLocation(FVector{MidPoint.X, MidPoint.Y, std::min(LeftFootPos.Z, RightFootPos.Z) + WaistHeight * 1.5f});
}

// Called when the game starts or when spawned
void ARG_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerState* PS = GetPlayerState();
	if (ARG_PlayerState* CastedPS = Cast<ARG_PlayerState>(GetPlayerState()))
		GamePlayerState = CastedPS;
	
	
	StartRagdoll();

	SavePlayerLocation();
}

// Called every frame
void ARG_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsPlayerTripped)
		TripTimer -= DeltaTime;
	
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
	// Enable Collisions
	ULiveRagdollHelperLib::EnableBoneLive(Skeleton, "Waist", FLiveRagdollBoneData{ 1.f, true, true});

	TArray<FString> KeysToRemove;
	
	// Create physics handles
	for (auto& LiveRigData : LiveRigTargetPoints)
	{
		if (FLiveRigBoneData* BoneData = LiveRigBoneData->BoneDataMap.Find(LiveRigData.Key))
		{
			FLiveRigTargetData& TargetData = LiveRigData.Value;

			// Get actual bone name
			TargetData.BoneTarget = BoneData->BoneName;

			// Create new physicsHandle
			TargetData.PhysicsHandle = NewObject<UPhysicsHandleComponent>(this, FName(*FString::Printf(TEXT("%s_PhysicsHandle"), *LiveRigData.Key)));
			TargetData.PhysicsHandle->RegisterComponent();
			TargetData.PhysicsHandle->InterpolationSpeed = HANDLE_INTERP;

			// Find Bone Position and grab it via physics components
			FVector BonePos = Skeleton->GetBoneLocation(TargetData.BoneTarget, EBoneSpaces::WorldSpace);
			TargetData.PhysicsHandle->GrabComponentAtLocation(Skeleton, TargetData.BoneTarget, BonePos);
		}
		else
			KeysToRemove.Add(LiveRigData.Key);
	}
	
	for (const FString& Key : KeysToRemove)
		LiveRigTargetPoints.Remove(Key);
	
	LiveRigTargetPoints["LeftHand"].PhysicsHandle->ReleaseComponent();
	LiveRigTargetPoints["RightHand"].PhysicsHandle->ReleaseComponent();
	LiveRigTargetPoints["Head"].PhysicsHandle->SetLinearDamping(0.5f);
	LiveRigTargetPoints["Head"].PhysicsHandle->SetLinearStiffness(.5f);
	LiveRigTargetPoints["Waist"].PhysicsHandle->SetLinearDamping(0.5f);
	LiveRigTargetPoints["Waist"].PhysicsHandle->SetLinearStiffness(500.f);
	
	// Find feet and thigh bones
	
	if ( const FLiveRigBoneData* Thigh = LiveRigBoneData->BoneDataMap.Find("LeftThigh"))
		if ( const FLiveRigBoneData* Foot = LiveRigBoneData->BoneDataMap.Find("LeftFoot"))
		{
			// freaky ass code
			const FVector ThighPos = Skeleton->GetBoneLocation(Thigh->BoneName, EBoneSpaces::WorldSpace);
			const FVector FeetPos = Skeleton->GetBoneLocation(Foot->BoneName, EBoneSpaces::WorldSpace);
			
			LegLength = UKismetMathLibrary::Vector_Distance(ThighPos, FeetPos);
			bThighLocated = true;
		}
	
	
	// Find Shoulder and Hand Bones
	if ( const FLiveRigBoneData* Shoulder = LiveRigBoneData->BoneDataMap.Find("LeftShoulder"))
		if ( const FLiveRigBoneData* Hand = LiveRigBoneData->BoneDataMap.Find("LeftHand"))
		{
			const FVector ShoulderPos = Skeleton->GetBoneLocation(Shoulder->BoneName, EBoneSpaces::WorldSpace);
			const FVector HandPos = Skeleton->GetBoneLocation(Hand->BoneName, EBoneSpaces::WorldSpace);
			
			ArmLength = UKismetMathLibrary::Vector_Distance(ShoulderPos, HandPos) * ARM_LENGTH_MULT;
			bShoulderLocated = true;
		}
}

void ARG_PlayerCharacter::Kill()
{
	if (!GamePlayerState || GamePlayerState->IsDead())
		return;

	if (GamePlayerState->IsInvincible())
		return;
	
	GamePlayerState->bIsAlive = false;
	OnPlayerDied.Broadcast();

	AudioPlayer->SetSound(DeathSound);
	AudioPlayer->Play();
	
	for (auto& LiveRigData : LiveRigTargetPoints)
	{
		FLiveRigTargetData& BoneData = LiveRigData.Value;
		BoneData.PhysicsHandle->ReleaseComponent();

		if (FBodyInstance* BodyInstance = Skeleton->GetBodyInstance(BoneData.BoneTarget))
		{
			FVector Velocity = BodyInstance->GetUnrealWorldVelocity();
			Velocity *= FUNNY_RAGDOLL_MULT;
			BodyInstance->SetLinearVelocity(Velocity, false);
		}
	}

	Boom->AttachToComponent(Skeleton, FAttachmentTransformRules::SnapToTargetNotIncludingScale, LiveRigTargetPoints["Waist"].BoneTarget);

	DeathCount++;

	if (bCanPlayerRespawn)
	{
		DropItem(false, true);
		DropItem(true, true);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ARG_PlayerCharacter::LoadPlayerLocation, TimeBeforeRespawn);
	}
		
}

void ARG_PlayerCharacter::Look(const FVector2D& Direction)
{
	AddControllerYawInput(Direction.X);
	AddControllerPitchInput(Direction.Y);
}


void ARG_PlayerCharacter::LiftLeg(const bool bRightLeg)
{
	if (!GamePlayerState || GamePlayerState->IsDead())
		return;
	
	if (bIsPlayerTripped)
		return;
	
	if (bThighLocated)
	{
		FVector ThighPos = Skeleton->GetBoneLocation(LiveRigBoneData->BoneDataMap[(bRightLeg) ? "RightThigh" : "LeftThigh"].BoneName, EBoneSpaces::WorldSpace);
		FVector EndPos = ThighPos + FVector(0.0f, 0.f, LegLength * LEG_LENGTH_HEIGHT_MULT);

		FVector& LastFootPos = (bRightLeg) ? RightFootLastPos : LeftFootLastPos;
		LastFootPos = LiveRigTargetPoints[(bRightLeg) ? "RightFoot" : "LeftFoot"].Target->GetComponentLocation();
		LiveRigTargetPoints[(bRightLeg) ? "RightFoot" : "LeftFoot"].Target->SetWorldLocation(EndPos);
		
		if (bRightLeg)
		{
			RightFootLiftedPos = EndPos;
			GamePlayerState->bRightLegLifted = true;
		}
		else
		{
			LeftFootLiftedPos = EndPos;
			GamePlayerState->bLeftLegLifted = true;
		}

		if (GamePlayerState->bLeftLegLifted && GamePlayerState->bRightLegLifted && !GamePlayerState->bHeadDetached)
		{
			GamePlayerState->bHeadDetached = true;
			LiveRigTargetPoints["Head"].PhysicsHandle->ReleaseComponent();
			LiveRigTargetPoints["Waist"].PhysicsHandle->ReleaseComponent();
			LiveRigTargetPoints["LeftFoot"].PhysicsHandle->ReleaseComponent();
			LiveRigTargetPoints["RightFoot"].PhysicsHandle->ReleaseComponent();
		}
	}
}

void ARG_PlayerCharacter::SteerFeet(const FVector2D& Direction)
{
	if (!GamePlayerState || GamePlayerState->IsDead())
		return;
	
	const FVector Forward = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	const FVector Right   = UKismetMathLibrary::GetRightVector(GetControlRotation());

	FVector TranslatedDirection =
		(Direction.X * Right) +
		(Direction.Y * Forward);

	TranslatedDirection.Z = 0.f;
	TranslatedDirection.Normalize();
	
	if (GamePlayerState->bLeftLegLifted)
	{
		LiveRigTargetPoints["LeftFoot"].Target->SetWorldLocation
		(LeftFootLiftedPos + TranslatedDirection * LegLength * LEG_LENGTH_DIST_MULT);
	}
	
	if (GamePlayerState->bRightLegLifted)
	{
		LiveRigTargetPoints["RightFoot"].Target->SetWorldLocation
		(RightFootLiftedPos + TranslatedDirection * LegLength * LEG_LENGTH_DIST_MULT);
	}
}

void ARG_PlayerCharacter::UpdateArmPos()
{
	if (!bShoulderLocated)
		return;
	
	FVector Forward = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	
	if (GamePlayerState->bLeftArmLifted)
	{
		FVector ShoulderPos = Skeleton->GetBoneLocation(LiveRigBoneData->BoneDataMap["LeftShoulder"].BoneName, EBoneSpaces::WorldSpace);
		LiveRigTargetPoints["LeftHand"].Target->SetWorldLocation(ShoulderPos + Forward * ArmLength);
	}
	
	if (GamePlayerState->bRightArmLifted)
	{
		FVector ShoulderPos = Skeleton->GetBoneLocation(LiveRigBoneData->BoneDataMap["RightShoulder"].BoneName, EBoneSpaces::WorldSpace);
		LiveRigTargetPoints["RightHand"].Target->SetWorldLocation(ShoulderPos + Forward * ArmLength);
	}
}

void ARG_PlayerCharacter::TripPlayer()
{
	bIsPlayerTripped = true;
	TripTimer = TripLength;
	
	Boom->AttachToComponent(Skeleton, FAttachmentTransformRules::SnapToTargetNotIncludingScale, LiveRigTargetPoints["Waist"].BoneTarget);
	
	LiveRigTargetPoints["Head"].PhysicsHandle->ReleaseComponent();
	LiveRigTargetPoints["Waist"].PhysicsHandle->ReleaseComponent();
	LiveRigTargetPoints["LeftFoot"].PhysicsHandle->ReleaseComponent();
	LiveRigTargetPoints["RightFoot"].PhysicsHandle->ReleaseComponent();
	
	UE_LOG(LogTemp, Warning, TEXT("ReleaseLumbs"));
}

void ARG_PlayerCharacter::UnTripPlayer()
{
	bIsPlayerTripped = false;
	
	bTripStepImmunity = 2;
	
	AttachTargetsToBoneLocations();
	
	Boom->AttachToComponent(LRT_Waist, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	GrabBone(LiveRigTargetPoints["Head"]);
	GrabBone(LiveRigTargetPoints["Waist"]);
	GrabBone(LiveRigTargetPoints["LeftFoot"]);
	GrabBone(LiveRigTargetPoints["RightFoot"]);
}

void ARG_PlayerCharacter::AddForce(const FVector& Force)
{
	Skeleton->AddForceToAllBodiesBelow(Force, LiveRigTargetPoints["Waist"].BoneTarget, true, true);
}

void ARG_PlayerCharacter::SavePlayerLocation()
{
	if (!GamePlayerState)
		return;
	
	GamePlayerState->RespawnData =
		FPlayerRespawnData{
			LRT_LeftFoot->GetComponentLocation(),
			LRT_RightFoot->GetComponentLocation(),
			LRT_Head->GetComponentLocation(),
			LRT_Waist->GetComponentLocation(),
			GetActorLocation()
		};
}

void ARG_PlayerCharacter::LoadPlayerLocation()
{
	if (!GamePlayerState)
		return;

	GamePlayerState->bIsAlive = true;
	GamePlayerState->bIsInvincible = true;

	OnPlayerRespawned.Broadcast();

	const FPlayerRespawnData& Data = GamePlayerState->RespawnData;
	SetActorLocation(Data.MainLocation);
	LRT_LeftFoot->SetWorldLocation(Data.LeftFootLocation);
	LRT_RightFoot->SetWorldLocation(Data.RightFootLocation);
	LRT_Waist->SetWorldLocation(Data.WaistLocation);
	LRT_Head->SetWorldLocation(Data.HeadLocation);

	GamePlayerState->bHeadDetached = false;
	GrabBone(LiveRigTargetPoints["Head"]);
	GrabBone(LiveRigTargetPoints["Waist"]);
	GrabBone(LiveRigTargetPoints["LeftFoot"]);
	GrabBone(LiveRigTargetPoints["RightFoot"]);

	GamePlayerState->bLeftLegLifted = false;
	GamePlayerState->bRightLegLifted = false;
	
	Boom->AttachToComponent(LRT_Waist, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, GamePlayerState, &ARG_PlayerState::RevertInvincibility, RespawnInvincibilityTime);
}

void ARG_PlayerCharacter::GrabBone(FLiveRigTargetData& RigData)
{
	FVector BoneLocation = Skeleton->GetBoneLocation(RigData.BoneTarget, EBoneSpaces::WorldSpace);
	RigData.PhysicsHandle->GrabComponentAtLocation(Skeleton, RigData.BoneTarget, BoneLocation);
}

void ARG_PlayerCharacter::AttachTargetsToBoneLocations()
{
	for (auto& LiveRigData : LiveRigTargetPoints)
	{
		FLiveRigTargetData& BoneData = LiveRigData.Value;
		
		const FVector BonePos = Skeleton->GetBoneLocation(BoneData.BoneTarget, EBoneSpaces::WorldSpace);
		BoneData.Target->SetWorldLocation(BonePos);
	}
}

void ARG_PlayerCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{
	if (!GamePlayerState || GamePlayerState->IsDead())
		return;
	
	if (bIsPlayerTripped)
	{
		if (TripTimer <= 0.f)
			UnTripPlayer();
		return;
	}
	
	if (HitResult.MyBoneName == LiveRigTargetPoints["Head"].BoneTarget)
	{
		if (bTripStepImmunity > 0)
			return;
		
		// Ignore items cause thats just not fun :P
		if (IsValid(OtherActor) && OtherActor->ActorHasTag("Item"))
			return;
		
		Kill();
		return;
	}

	if (IsValid(OtherActor) && OtherActor->ActorHasTag("Item"))
	{
		const uint8 GrabbingHand = GamePlayerState->GetGrabbingArm();

		// If no valid hand give up
		if (!GrabbingHand)
			return;
		
		FName GrabbingSocket = (GrabbingHand == 1) ? FName("LeftHand_Socket") : FName("RightHand_Socket");
		ARG_ItemBase*& PlayerStateValue = (GrabbingHand == 1) ? GamePlayerState->LeftHandItem : GamePlayerState->RightHandItem;
		
		if (ARG_ItemBase* ItemActor = Cast<ARG_ItemBase>(OtherActor))
		{
			PlayerStateValue = ItemActor;
			ItemActor->Grab(this, Skeleton, GrabbingSocket);
		}
	}
	
}

void ARG_PlayerCharacter::DropLeg(const bool bRightLeg)
{
	if (!GamePlayerState || GamePlayerState->IsDead())
		return;
	
	if (bTripStepImmunity)
		bTripStepImmunity--;
	
	const FVector StartingPos = LiveRigTargetPoints[(bRightLeg) ? "RightFoot" : "LeftFoot"].Target->GetComponentLocation();
	
	bool bDidHit;
	const FVector EndPos = UCommonBlueprintFunctionLibrary::GetFirstHitLocation(this, StartingPos, FVector{0.f, 0.f, -1.f}, 
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn), bDidHit, TArray<AActor*>{this});
	
	
	if (bDidHit)
	{
		LiveRigTargetPoints[(bRightLeg) ? "RightFoot" : "LeftFoot"].Target->SetWorldLocation(EndPos);
		UpdateTargetPositions();
	}
	else
	{
		FVector& LastFootPos = (bRightLeg) ? RightFootLastPos : LeftFootLastPos;
		LiveRigTargetPoints[(bRightLeg) ? "RightFoot" : "LeftFoot"].Target->SetWorldLocation(LastFootPos);
		UpdateTargetPositions();
	}
	
	if (bRightLeg)
		GamePlayerState->bRightLegLifted = false;
	else
		GamePlayerState->bLeftLegLifted = false;

	if (GamePlayerState->bHeadDetached)
	{
		GamePlayerState->bHeadDetached = false;
		GrabBone(LiveRigTargetPoints["Head"]);
		GrabBone(LiveRigTargetPoints["Waist"]);
		GrabBone(LiveRigTargetPoints["LeftFoot"]);
		GrabBone(LiveRigTargetPoints["RightFoot"]);
	}
}

void ARG_PlayerCharacter::LiftArm(const bool bRightArm)
{
	if (!GamePlayerState || GamePlayerState->IsDead())
		return;
	
	GrabBone(LiveRigTargetPoints[(bRightArm) ? "RightHand" : "LeftHand"]);

	if (ARG_ItemBase* Item = (bRightArm) ? GamePlayerState->RightHandItem : GamePlayerState->LeftHandItem)
		Item->Activate();
	
	
	if (bRightArm)
		GamePlayerState->bRightArmLifted = true;
	else
		GamePlayerState->bLeftArmLifted = true;
	
}

void ARG_PlayerCharacter::DropArm(const bool bRightArm)
{
	if (!GamePlayerState || GamePlayerState->IsDead())
		return;
	
	LiveRigTargetPoints[(bRightArm) ? "RightHand" : "LeftHand"].PhysicsHandle->ReleaseComponent();

	if (ARG_ItemBase* Item = (bRightArm) ? GamePlayerState->RightHandItem : GamePlayerState->LeftHandItem)
		Item->Deactivate();
	
	if (bRightArm)
		GamePlayerState->bRightArmLifted = false;
	else
		GamePlayerState->bLeftArmLifted = false;
}

void ARG_PlayerCharacter::DropItem(const bool bRightArm, const bool bForceDrop)
{
	if (!GamePlayerState)
		return;

	if (!bForceDrop && GamePlayerState->IsDead())
		return;

	if (ARG_ItemBase*& Item = (bRightArm) ? GamePlayerState->RightHandItem : GamePlayerState->LeftHandItem; !Item)
		return;
	else
	{
		Item->Drop();
		Item = nullptr;
	}
		
}


