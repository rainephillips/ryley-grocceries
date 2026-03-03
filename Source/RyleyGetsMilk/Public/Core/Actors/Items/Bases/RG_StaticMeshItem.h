// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/Items/RG_ItemBase.h"
#include "RG_StaticMeshItem.generated.h"

UCLASS()
class RYLEYGETSMILK_API ARG_StaticMeshItem : public ARG_ItemBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBoxComponent* Box;
	
public:
	// Sets default values for this actor's properties
	ARG_StaticMeshItem();
};
