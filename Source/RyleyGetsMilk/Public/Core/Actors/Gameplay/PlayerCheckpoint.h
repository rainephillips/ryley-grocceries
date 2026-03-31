// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCheckpoint.generated.h"

UCLASS()
class RYLEYGETSMILK_API APlayerCheckpoint : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USceneComponent* SaveLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBoxComponent* Zone;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* Mesh;

public:
	// Sets default values for this actor's properties
	APlayerCheckpoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
