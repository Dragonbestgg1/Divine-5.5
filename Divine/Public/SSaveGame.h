// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SSaveGame.generated.h"


USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FName ActorName;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TArray<uint8> ByteData;
};

USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()

public:

	FPlayerSaveData()
	{
		Credits = 0;
		PersonalRecordTime = 0.0f;
		Health = 100.0f;
		Location = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
		bResumeAtTransform = true;
	}

	UPROPERTY()
	FString PlayerID;

	UPROPERTY()
	int32 Credits;

	UPROPERTY()
	float PersonalRecordTime;

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	float Health;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	bool bResumeAtTransform;
};

UCLASS()
class DIVINE_API USSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FPlayerSaveData> SavedPlayers;

	UPROPERTY()
	TArray<FActorSaveData> SavedActors;

	FPlayerSaveData* GetPlayerData(APlayerState* PlayerState);
};
