#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SSaveGameSubsystem.generated.h"

class USSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameSignature, class USSaveGame*, SaveObject);

UCLASS(meta = (DisplayName = "SaveGame System"))
class DIVINE_API USSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:

	FString CurrentSlotName;

	UPROPERTY()
	TObjectPtr<USSaveGame> CurrentSaveGame;

public:

	void HandleStartingNewPlayer(AController* NewPlayer);

	UFUNCTION(BlueprintCallable)
	bool OverrideSpawnTransform(AController* NewPlayer);

	UFUNCTION(BlueprintCallable)
	void SetSlotName(FString NewSlotName);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void LoadSaveGame(FString InSlotName = "");

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool HasValidSave() const { return CurrentSaveGame != nullptr; }


	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameWritten;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

};
