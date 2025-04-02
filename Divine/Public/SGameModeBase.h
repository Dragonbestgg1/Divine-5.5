#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Engine/DataTable.h"
#include "FEnemySpawnInfo.h"
#include "SGameModeBase.generated.h"

// Forward declarations:
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
    /* All available monsters */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TObjectPtr<UDataTable> MonsterTable;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TObjectPtr<UEnvQuery> SpawnBotQuery;

    /* Time to wait between failed attempts to spawn/buy monster to give some time to build up credits. */
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float CooldownTimeBetweenFailures;

    FTimerHandle TimerHandle_SpawnBots;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float SpawnTimerInterval;

    /* Amount available to start spawning some bots immediately */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
    int32 InitialSpawnCredit;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
    bool bAutoStartBotSpawning;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ruleset")
    bool bAutoRespawnPlayer;

    UPROPERTY(EditDefaultsOnly, Category = "Powerups")
    TObjectPtr<UEnvQuery> PowerupSpawnQuery;

    /* All power-up classes used to spawn with EQS at match start */
    UPROPERTY(EditDefaultsOnly, Category = "Powerups")
    TArray<TSubclassOf<AActor>> PowerupClasses;

    /* Distance required between power-up spawn locations */
    UPROPERTY(EditDefaultsOnly, Category = "Powerups")
    float RequiredPowerupDistance;

    /* Amount of powerups to spawn during match start */
    UPROPERTY(EditDefaultsOnly, Category = "Powerups")
    int32 DesiredPowerupCount;

    UFUNCTION(BlueprintCallable)
    void StartSpawningBots();

    void SpawnBotTimerElapsed();

    /** Called when the EnvQuery finishes for bot spawns */
    void OnBotSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result);

    /** Called when the EnvQuery finishes for powerup spawns */
    void OnPowerupSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result);

    UFUNCTION()
    void RespawnPlayerElapsed(AController* Controller);

    /* GameTime cooldown to give spawner some time to build up credits */
    float CooldownBotSpawnUntil;

    // Store the monster row selected from the MonsterTable
    FEnemySpawnInfo* SelectedMonsterRow;

    UPROPERTY(EditDefaultsOnly, Category = "Actor Pooling")
    TMap<TSubclassOf<AActor>, int32> ActorPoolClasses;

    // Request to prime actors (unused if you haven't set up actor pooling).
    void RequestPrimedActors();

    UFUNCTION(BlueprintCallable, Category = "Level Progression")
    void OnNewLevelStarted();

    /** Tracks the current level number */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Progression")
    int32 CurrentLevel;

    /** Function to adjust spawn rate */
    void UpdateSpawnRateForLevel();

    /** Adjust spawn credits dynamically per level */
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

    // Calculate spawn credit based on current level
    float CalculateSpawnCredit(int32 Level);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
    TSubclassOf<AItemManager> ItemManagerClass;

    UPROPERTY()
    AItemManager* ItemManager;

    // >>> NEW FUNCTION DECLARATION FOR SCALING SPAWN <<<
    void SpawnScaledEnemy(const FVector& SpawnLocation);
    // <<<

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
