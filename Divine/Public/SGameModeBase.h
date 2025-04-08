#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Engine/DataTable.h"
#include "FEnemySpawnInfo.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UDataTable;
class UUserWidget;
class ALevelTransitionActor;
class AItemManager;

UCLASS()
class DIVINE_API ASGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TObjectPtr<UDataTable> MonsterTable;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TObjectPtr<UEnvQuery> SpawnBotQuery;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float CooldownTimeBetweenFailures;

    FTimerHandle TimerHandle_SpawnBots;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float SpawnTimerInterval;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
    int32 InitialSpawnCredit;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
    bool bAutoStartBotSpawning;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ruleset")
    bool bAutoRespawnPlayer;

    UPROPERTY(EditDefaultsOnly, Category = "Powerups")
    TObjectPtr<UEnvQuery> PowerupSpawnQuery;

    UPROPERTY(EditDefaultsOnly, Category = "Powerups")
    TArray<TSubclassOf<AActor>> PowerupClasses;

    UPROPERTY(EditDefaultsOnly, Category = "Powerups")
    float RequiredPowerupDistance;

    UPROPERTY(EditDefaultsOnly, Category = "Powerups")
    int32 DesiredPowerupCount;

    UFUNCTION(BlueprintCallable)
    void StartSpawningBots();

    void SpawnBotTimerElapsed();

    void OnBotSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result);

    void OnPowerupSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result);

    UFUNCTION()
    void RespawnPlayerElapsed(AController* Controller);

    float CooldownBotSpawnUntil;

    FEnemySpawnInfo* SelectedMonsterRow;

    UPROPERTY(EditDefaultsOnly, Category = "Actor Pooling")
    TMap<TSubclassOf<AActor>, int32> ActorPoolClasses;

    void RequestPrimedActors();

    UFUNCTION(BlueprintCallable, Category = "Level Progression")
    void OnNewLevelStarted();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Progression")
    int32 CurrentLevel;

    void UpdateSpawnRateForLevel();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning")
    float AvailableSpawnCredit;

    UFUNCTION()
    void OnSubLevelLoaded();

    UPROPERTY()
    UUserWidget* LoadingScreenWidget;

    void SpawnInitialEnemies();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> LoadingScreenWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> CompletedScreenWidgetClass;

    UUserWidget* CompletedScreenWidget;

    bool bIsTransitioning;

    FTimerHandle LevelLoadTimerHandle;

    float CalculateSpawnCredit(int32 Level);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
    TSubclassOf<AItemManager> ItemManagerClass;

    UPROPERTY()
    AItemManager* ItemManager;

    void SpawnScaledEnemy(const FVector& SpawnLocation);

public:
    ASGameModeBase();

    UFUNCTION(BlueprintCallable, Category = "Level Progression")
    void OnLevelChanged();

    UPROPERTY(EditDefaultsOnly, Category = "Level Transition")
    TSubclassOf<ALevelTransitionActor> LevelTransitionActorClass;

    UFUNCTION(BlueprintCallable, Category = "Level")
    int32 GetCurrentLevel() const { return CurrentLevel; }

    UFUNCTION(BlueprintCallable)
    void LoadSubLevelAsync(FName LevelName);

    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void StartPlay() override;
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    UFUNCTION(BlueprintCallable)
    void OnActorKilled(AActor* VictimActor, AActor* Killer);

    UFUNCTION(Exec)
    void KillAll();
};
