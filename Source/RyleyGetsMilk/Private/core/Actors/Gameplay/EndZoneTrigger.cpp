// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Actors/Gameplay/EndZoneTrigger.h"

#include "Components/BoxComponent.h"

#include "Core/Actors/Player/RG_PlayerCharacter.h"
#include "Core/Actors/Player/RG_PlayerState.h"

// Sets default values
AEndZoneTrigger::AEndZoneTrigger()
{
	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone"));
	SetRootComponent(Zone);

	Zone->SetCollisionProfileName(TEXT("Custom"));
	Zone->SetCollisionResponseToAllChannels(ECR_Ignore);
	Zone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Zone->SetGenerateOverlapEvents(true);

	Zone->OnComponentBeginOverlap.AddDynamic(this, &AEndZoneTrigger::OnOverlap);
}

// Called when the game starts or when spawned
void AEndZoneTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEndZoneTrigger::OnPlayerOverlap_Implementation()
{
}

void AEndZoneTrigger::OnEndConditionsMet_Implementation()
{
}

void AEndZoneTrigger::OnOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
                                class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor->ActorHasTag("Player"))
	{
		OnPlayerOverlap();
		
		if (ARG_PlayerCharacter* PlayerCharacter = Cast<ARG_PlayerCharacter>(OtherActor))
		{
			if (PlayerCharacter->GamePlayerState->HasMilk())
			{
				OnEndConditionsMet();
			}
		}
	}
}

