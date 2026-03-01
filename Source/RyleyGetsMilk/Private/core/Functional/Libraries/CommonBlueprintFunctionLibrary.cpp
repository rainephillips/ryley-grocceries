// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Functional/Libraries/CommonBlueprintFunctionLibrary.h"
#include "KismetTraceUtils.h"
#include "GameFramework/PlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void UCommonBlueprintFunctionLibrary::SetGameInputMode(UObject* WorldContextObject, int32 PlayerID, bool bShowMouse)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerID))
	{
		FInputModeGameOnly GameInput;

		PC->SetInputMode(GameInput);
		PC->SetShowMouseCursor(bShowMouse);
	}
}

void UCommonBlueprintFunctionLibrary::SetUIInputMode(UObject* WorldContextObject, EMouseLockMode MouseLock,
	UUserWidget* Focus, int32 PlayerID, bool bShowMouse, bool bFlushControls)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerID))
	{
		FInputModeUIOnly UIInput;

		UIInput.SetLockMouseToViewportBehavior(MouseLock);
		if (IsValid(Focus))
			UIInput.SetWidgetToFocus(Focus->GetCachedWidget());
		PC->SetInputMode(UIInput);
		PC->SetShowMouseCursor(bShowMouse);
		if (bFlushControls)
			PC->FlushPressedKeys();
	}
}

void UCommonBlueprintFunctionLibrary::SetGameAndUIInputMode(UObject* WorldContextObject, EMouseLockMode MouseLock,
	UUserWidget* Focus, int32 PlayerID, bool bShowMouse, bool bFlushControls)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerID))
	{
		FInputModeGameAndUI GameAndUIInput;
		GameAndUIInput.SetLockMouseToViewportBehavior(MouseLock);
		PC->SetInputMode(GameAndUIInput);
		PC->SetShowMouseCursor(bShowMouse);
		if (IsValid(Focus))
		{
			GameAndUIInput.SetWidgetToFocus(Focus->GetCachedWidget());
		}
	}
}

void UCommonBlueprintFunctionLibrary::LineTraceFromMouse(APlayerController* PlayerController, float Distance, ETraceTypeQuery TraceChannel,
	TArray<AActor*> Ignored, bool bTraceComplex, bool bShowHits, TArray<FHitResult>& Hits, FHitResult& Hit,
	bool& bDidHit)
{
	const UWorld* World = PlayerController->GetWorld();
	
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = bTraceComplex;
	
	FVector Start;
	FVector Direction;
	if (!PlayerController->DeprojectMousePositionToWorld(Start,Direction))
	{
		bDidHit = false;
		return;
	}
	
	const FVector End = Start + Direction * Distance;
	
	
	
#if ENABLE_DRAW_DEBUG
	QueryParams.bDebugQuery = bShowHits;
#endif
	QueryParams.AddIgnoredActors(Ignored);

	bDidHit = World->LineTraceMultiByChannel(Hits, Start, End, UEngineTypes::ConvertToCollisionChannel(TraceChannel), QueryParams);

#if ENABLE_DRAW_DEBUG
	const EDrawDebugTrace::Type DrawDebugType = bShowHits ?
		EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	DrawDebugLineTraceMulti(
		World, Start, End, DrawDebugType, bDidHit,
		Hits, FLinearColor::Blue, FLinearColor::Yellow, 5.0f);
#endif
}

bool UCommonBlueprintFunctionLibrary::GetMousePosInWorldCoordinates(APlayerController* PlayerController,
	FVector& WorldLocation, bool bShowDebug )
{
	FVector Start;
	FVector Direction;
	if (!PlayerController->DeprojectMousePositionToWorld(Start,Direction))
		return false;
	
	const UWorld* World = PlayerController->GetWorld();
	
	const FVector End = Start + Direction * 10000.0f;
	
	FCollisionQueryParams QueryParams;
#if ENABLE_DRAW_DEBUG
	QueryParams.bDebugQuery = bShowDebug;
#endif
	FHitResult HitResult;
	
	bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		QueryParams
	);
	
#if ENABLE_DRAW_DEBUG
	DrawDebugLineTraceSingle(
		World, Start, End, bShowDebug? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, true,
		HitResult, FLinearColor::Blue, FLinearColor::Yellow, 5.0f);
#endif
	
	if (!bHit)
		return false;
	
	WorldLocation = HitResult.Location;
	
	return true;
}


