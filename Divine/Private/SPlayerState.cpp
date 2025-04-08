#include "SPlayerState.h"
#include "Divine.h"
#include "SSaveGame.h"
#include "SAttributeComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SPlayerState)

ASPlayerState::ASPlayerState()
{

	Credits = 500;
	PersonalRecordTime = 0.0f;
	PlayerHealth = 100.0f;
}

void ASPlayerState::AddCredits(int32 Delta)
{
	if (!ensure(Delta >= 0))
	{
		return;
	}

	Credits += Delta;
	OnCreditsChanged.Broadcast(this, Credits, Delta);
}

void ASPlayerState::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(ScoreTickTimer, this, &ASPlayerState::TickScoreDown, 1.0f, true);
}

void ASPlayerState::TickScoreDown()
{
	if (Credits > 0)
	{
		Credits -= 1;
		OnCreditsChanged.Broadcast(this, Credits, -1);
	}
}

bool ASPlayerState::RemoveCredits(int32 Delta)
{
	if (!ensure(Delta >= 0))
	{
		return false;
	}

	if (Credits < Delta)
	{
		return false;
	}

	Credits -= Delta;
	OnCreditsChanged.Broadcast(this, Credits, -Delta);
	return true;
}

void ASPlayerState::ResetCredits()
{
	Credits = 0;
	OnCreditsChanged.Broadcast(this, Credits, 0);
}

bool ASPlayerState::UpdatePersonalRecord(float NewTime)
{
	if (NewTime > PersonalRecordTime)
	{
		float OldRecord = PersonalRecordTime;
		PersonalRecordTime = NewTime;
		OnRecordTimeChanged.Broadcast(this, PersonalRecordTime, OldRecord);
		return true;
	}
	return false;
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		FPlayerSaveData SaveData;
		SaveData.Credits = Credits;
		SaveData.PersonalRecordTime = PersonalRecordTime;

		if (APawn* MyPawn = GetPawn())
		{
			if (USAttributeComponent* AttrComp = MyPawn->FindComponentByClass<USAttributeComponent>())
			{
				SaveData.Health = AttrComp->GetHealth();
			}
			else
			{
				SaveData.Health = PlayerHealth;
			}

			SaveData.Location = MyPawn->GetActorLocation();
			SaveData.Rotation = MyPawn->GetActorRotation();
			SaveData.bResumeAtTransform = true;
		}
		SaveData.PlayerID = TEXT("DefaultPlayer");

		SaveObject->SavedPlayers.Add(SaveData);
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		FPlayerSaveData* FoundData = SaveObject->GetPlayerData(this);
		if (FoundData)
		{
			AddCredits(FoundData->Credits);
			PersonalRecordTime = FoundData->PersonalRecordTime;
			PlayerHealth = FoundData->Health;

			if (APawn* MyPawn = GetPawn())
			{
				if (USAttributeComponent* AttrComp = MyPawn->FindComponentByClass<USAttributeComponent>())
				{
					AttrComp->SetCurrentHealth(FoundData->Health);
				}
			}
			else
			{
				if (UWorld* World = GetWorld())
				{
					FTimerHandle TimerHandle;
					World->GetTimerManager().SetTimer(TimerHandle, [this, FoundData]()
						{
							if (APawn* DelayedPawn = GetPawn())
							{
								if (USAttributeComponent* AttrComp = DelayedPawn->FindComponentByClass<USAttributeComponent>())
								{
									AttrComp->SetCurrentHealth(FoundData->Health);
								}
							}
						}, 0.5f, false);
				}
			}
		}
	}
}

void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits - OldCredits);
}

int32 ASPlayerState::GetCredits() const
{
	return Credits;
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASPlayerState, Credits);
}
