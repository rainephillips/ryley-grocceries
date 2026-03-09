// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/Items/RG_ItemBase.h"
#include "RG_GunItemBase.generated.h"

UCLASS()
class RYLEYGETSMILK_API ARG_GunItemBase : public ARG_ItemBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Item Data")
	float Damage = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Item Data")
	TSubclassOf<AActor> BulletClass;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USceneComponent* Barrel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USceneComponent* Pivot;

public:
	// Sets default values for this actor's properties
	ARG_GunItemBase();

public:
	UFUNCTION(BlueprintNativeEvent)
	void Shoot();
};
