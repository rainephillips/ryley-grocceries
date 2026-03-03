// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Actors/Items/Bases/RG_GunItemBase.h"

#include "Core/Functional/Interfaces/Shootable.h"
#include "Core/Functional/Libraries/CommonBlueprintFunctionLibrary.h"

// Sets default values
ARG_GunItemBase::ARG_GunItemBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Pivot = CreateDefaultSubobject<USceneComponent>("Pivot");
	SetRootComponent(Pivot);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Pivot);

	Barrel = CreateDefaultSubobject<USceneComponent>("Barrel");
	Barrel->SetupAttachment(Mesh);
	
	OnItemUsed.AddDynamic(this, &ARG_GunItemBase::Shoot);
}

void ARG_GunItemBase::Shoot_Implementation()
{
	TArray<FHitResult> Hits;
	FHitResult FirstHit;
	bool bDidHit = false;
	
	UCommonBlueprintFunctionLibrary::LineTraceInFrontOfObject(Barrel, FVector::ZeroVector, 10000.f, FRotator::ZeroRotator,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn), TArray<AActor*>{ItemUser}, true, true, Hits, FirstHit, bDidHit);

	if (!bDidHit)
		return;

	for (FHitResult& HitResult : Hits)
	{
		AActor* HitActor = HitResult.GetActor();
		
		if (IsValid(HitActor) && HitActor->Implements<UShootable>())
		{
			IShootable* Shootable = Cast<IShootable>(HitActor);

			if (Shootable->Execute_IsShootable(HitActor, ItemUser))
				Shootable->Execute_DealDamage(HitActor, Damage);

			break;
		}
	}
}

