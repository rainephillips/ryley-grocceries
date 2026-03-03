// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndZoneTrigger.generated.h"

UCLASS()
class RYLEYGETSMILK_API AEndZoneTrigger : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBoxComponent* Zone;

public:
	// Sets default values for this actor's properties
	AEndZoneTrigger();

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnPlayerOverlap();

	UFUNCTION(BlueprintNativeEvent)
	void OnEndConditionsMet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
