// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Logging/StructuredLog.h"

DECLARE_STATS_GROUP(TEXT("STANFORD_Game"), STATGROUP_STANFORD, STATCAT_Advanced);

DIVINE_API DECLARE_LOG_CATEGORY_EXTERN(LogGame, Log, All);

#define COLLISION_PROJECTILE ECC_GameTraceChannel1


static void LogOnScreen(UObject* WorldContext, FString Msg, FColor Color = FColor::White, float Duration = 5.0f)
{
	if (!ensure(WorldContext))
	{
		return;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!ensure(World))
	{
		return;
	}

	FString NetPrefix = World->IsNetMode(NM_Client) ? "[CLIENT] " : "[SERVER] ";
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, NetPrefix + Msg);
	}
}