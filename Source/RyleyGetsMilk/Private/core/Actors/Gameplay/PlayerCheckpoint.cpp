// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Actors/Gameplay/PlayerCheckpoint.h"

#include "Components/BoxComponent.h"

#include "Core/Actors/Player/RG_PlayerCharacter.h"
#include "Core/Actors/Player/RG_PlayerState.h"

// Sets default values
APlayerCheckpoint::APlayerCheckpoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone"));
	Zone->SetupAttachment(Root);
	Zone->SetCollisionProfileName(TEXT("Custom"));
	Zone->SetCollisionResponseToAllChannels(ECR_Ignore);
	Zone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Zone->SetGenerateOverlapEvents(true);

	Zone->OnComponentBeginOverlap.AddDynamic(this, &APlayerCheckpoint::OnOverlap);
}

// Called when the game starts or when spawned
void APlayerCheckpoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCheckpoint::OnOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor->ActorHasTag("Player"))
	{
		if (ARG_PlayerCharacter* PlayerCharacter = Cast<ARG_PlayerCharacter>(OtherActor))
		{
			if (PlayerCharacter->GamePlayerState->IsAlive())
			{
				PlayerCharacter->AttachTargetsToBoneLocations();
				PlayerCharacter->SavePlayerLocation();
			}
		}
	}
}

