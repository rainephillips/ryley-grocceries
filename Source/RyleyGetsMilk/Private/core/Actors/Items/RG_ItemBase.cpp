// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Actors/Items/RG_ItemBase.h"

#include "Core/Actors/Player/RG_PlayerCharacter.h"
#include "Core/Functional/Libraries/LiveRagdollHelperLib.h"

// Sets default values
ARG_ItemBase::ARG_ItemBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add("Item");
}

void ARG_ItemBase::Grab(class APawn* User, class UPrimitiveComponent* Attached, const FName& SocketName)
{
	ULiveRagdollHelperLib::AttachActorToSocketByName(this, Attached, SocketName, true, true, false);
	ItemUser = User;
	OnItemGrabbed.Broadcast();

	Activate();
}

void ARG_ItemBase::Drop()
{
	OnItemDropped.Broadcast(ItemUser);
	ItemUser = nullptr;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorEnableCollision(true);

	Deactivate();
}

void ARG_ItemBase::Activate()
{
	bIsActivated = true;
	
	if (bSingleUse)
	{
		OnItemUsed.Broadcast();
		return;
	}
	
	SetActorTickEnabled(true);
	UseTimer = UseLength;
}

void ARG_ItemBase::Deactivate()
{
	bIsActivated = false;
	SetActorTickEnabled(false);
}

void ARG_ItemBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& HitResult)
{
	if (ARG_PlayerCharacter* PlayerActor = Cast<ARG_PlayerCharacter>(OtherActor))
	{
		if (!PlayerActor || !PlayerActor->ActorHasTag("Player"))
			return;
	}
}

// Called when the game starts or when spawned
void ARG_ItemBase::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

// Called every frame
void ARG_ItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UseTimer -= DeltaTime;
	if (UseTimer <= 0)
	{
		UseTimer = UseLength;
		OnItemUsed.Broadcast();
	}
}

