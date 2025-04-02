// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGame.h"

#include "Divine.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SSaveGame)


FPlayerSaveData* USSaveGame::GetPlayerData(APlayerState* PlayerState)
{
	if (PlayerState == nullptr)
	{
		return nullptr;
	}

	if (PlayerState->GetWorld()->IsPlayInEditor())
	{

		if (SavedPlayers.IsValidIndex(0))
		{
			return &SavedPlayers[0];
		}

		UE_LOGFMT(LogGame, Log, "Couldn't get Player ID.");

		// No saved player data available
		return nullptr;
	}

	return SavedPlayers.FindByPredicate([](const FPlayerSaveData& Data) { return Data.PlayerID == TEXT("DefaultPlayer"); });

}
