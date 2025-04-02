// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGameSubsystem.h"

#include "Divine.h"
#include "EngineUtils.h"
#include "SGameplayInterface.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "SSaveGameSettings.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SSaveGameSubsystem)



void USSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const USSaveGameSettings* SGSettings = GetDefault<USSaveGameSettings>();
	// Access defaults from DefaultGame.ini
	CurrentSlotName = SGSettings->SaveSlotName;

	UDataTable* DummyTable = SGSettings->DummyTablePath.LoadSynchronous();
}


void USSaveGameSubsystem::HandleStartingNewPlayer(AController* NewPlayer)
{
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (ensure(PS))
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}
}


bool USSaveGameSubsystem::OverrideSpawnTransform(AController* NewPlayer)
{
	if (!CurrentSaveGame)  // If there’s no valid save, just return false
	{
		return false;
	}

	if (!IsValid(NewPlayer))
	{
		return false;
	}

	APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>();
	if (PS == nullptr)
	{
		return false;
	}

	if (APawn* MyPawn = PS->GetPawn())
	{
		FPlayerSaveData* FoundData = CurrentSaveGame->GetPlayerData(PS);
		if (FoundData && FoundData->bResumeAtTransform)
		{
			MyPawn->SetActorLocation(FoundData->Location);
			MyPawn->SetActorRotation(FoundData->Rotation);

			// PlayerState owner is a (Player)Controller
			AController* PC = CastChecked<AController>(PS->GetOwner());
			PC->SetControlRotation(FoundData->Rotation);

			return true;
		}
	}

	return false;
}


void USSaveGameSubsystem::SetSlotName(FString NewSlotName)
{
	if (NewSlotName.Len() == 0)
	{
		return;
	}

	CurrentSlotName = NewSlotName;
}


void USSaveGameSubsystem::WriteSaveGame()
{

	if (!CurrentSaveGame)
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
	}

	// Now it is safe to clear arrays
	CurrentSaveGame->SavedPlayers.Empty();
	CurrentSaveGame->SavedActors.Empty();

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS == nullptr)
	{
		return;
	}

	for (int32 i = 0; i < GS->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = CastChecked<ASPlayerState>(GS->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			break;
		}
	}

	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (!IsValid(Actor) || !Actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetFName();
		ActorData.Transform = Actor->GetActorTransform();

		// Pass the array to fill with data from Actor
		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);

		Ar.ArIsSaveGame = true;
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);

	OnSaveGameWritten.Broadcast(CurrentSaveGame);
}


void USSaveGameSubsystem::LoadSaveGame(FString InSlotName /*= ""*/)
{
	SetSlotName(InSlotName);

	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOGFMT(LogGame, Warning, "Failed to load SaveGame Data.");
			return;
		}

		UE_LOG(LogGame, Log, TEXT("Loaded SaveGame Data from slot: %s"), *CurrentSlotName);
		UE_LOG(LogGame, Log, TEXT("SavedPlayers count: %d, SavedActors count: %d"),
			CurrentSaveGame->SavedPlayers.Num(), CurrentSaveGame->SavedActors.Num());



		for (AActor* Actor : TActorRange<AActor>(GetWorld()))
		{
			if (!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetFName())
				{
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemReader(ActorData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					Ar.ArIsSaveGame = true;
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}
		}

		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	}
	else
	{
		CurrentSaveGame = CastChecked<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

		UE_LOGFMT(LogGame, Log, "Created New SaveGame Data.");
	}
}
