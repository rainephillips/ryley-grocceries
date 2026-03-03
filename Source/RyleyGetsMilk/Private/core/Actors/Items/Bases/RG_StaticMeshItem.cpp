// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Actors/Items/Bases/RG_StaticMeshItem.h"

#include "Components/BoxComponent.h"

// Sets default values
ARG_StaticMeshItem::ARG_StaticMeshItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(Box);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Box);
	
	
	Collider = Box;
}

