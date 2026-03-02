// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommonBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RYLEYGETSMILK_API UCommonBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Input Helper", meta=(WorldContext="WorldContextObject"))
	static void SetGameInputMode(UObject* WorldContextObject, int32 PlayerID, bool bShowMouse = false);

	UFUNCTION(BlueprintCallable, Category="Input Helper", meta=(WorldContext="WorldContextObject"))
	static void SetUIInputMode(UObject* WorldContextObject, EMouseLockMode MouseLock, UUserWidget* Focus, int32 PlayerID, bool bShowMouse = true, bool bFlushControls = true);
	
	UFUNCTION(BlueprintCallable, Category="Input Helper", meta=(WorldContext="WorldContextObject"))
	static void SetGameAndUIInputMode(UObject* WorldContextObject, EMouseLockMode MouseLock, UUserWidget* Focus, int32 PlayerID, bool bShowMouse = true, bool bFlushControls = true);
	
	UFUNCTION(BlueprintCallable, Category="Tracing")
	static void LineTraceFromMouse(APlayerController* PlayerController, float Distance, ETraceTypeQuery TraceChannel, TArray<AActor*> Ignored, bool bTraceComplex, bool bShowHits,
								TArray<FHitResult>& Hits, FHitResult& Hit, bool& bDidHit);
	
	UFUNCTION(BlueprintCallable, Category="Input Helper")
	static bool GetMousePosInWorldCoordinates(APlayerController* PlayerController, FVector& WorldLocation, bool bShowDebug = false);
	
	UFUNCTION(BlueprintCallable, Category="Tracing", meta=(WorldContext="WorldContextObject"))
	static FVector GetFirstHitLocation(UObject* WorldContextObject, const FVector& Start, const FVector& Direction, ETraceTypeQuery TraceChannel, bool& bDidHit, TArray<AActor*> Ignored, bool bShowHits = false, bool bTraceComplex = false );
};
