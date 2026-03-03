// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RG_ItemBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemGrabbed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemDropped, APawn*, ItemUser);

UCLASS()
class RYLEYGETSMILK_API ARG_ItemBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
	bool bIsActivated = false;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
	bool bSingleUse = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
	APawn* ItemUser = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	float UseLength = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
	float UseTimer = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Events")
	FOnItemUsed OnItemUsed;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Events")
	FOnItemGrabbed OnItemGrabbed;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Events")
	FOnItemDropped OnItemDropped;

public:
	UFUNCTION(BlueprintCallable)
	inline bool IsActive() const {return bIsActivated;}
	

public:
	// Sets default values for this actor's properties
	ARG_ItemBase();

public:
	UFUNCTION(BlueprintCallable)
	void Grab(class APawn* User, class UPrimitiveComponent* Attached, const FName& SocketName = NAME_None);

	UFUNCTION(BlueprintCallable)
	void Drop();

	UFUNCTION(BlueprintCallable)
	virtual void Activate();

	UFUNCTION(BlueprintCallable)
	void Deactivate();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
		FVector NormalImpulse, const FHitResult& HitResult);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UPrimitiveComponent* Collider;
};
