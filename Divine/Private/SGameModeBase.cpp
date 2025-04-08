#include "SGameModeBase.h"
#include "FEnemySpawnInfo.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "AI/EnemyCharacter.h"
#include "SAttributeComponent.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "BasePlayer.h"
#include "SPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Divine.h"
#include "SActionComponent.h"
#include "SSaveGameSubsystem.h"
#include "Subsystems/SActorPoolingSubsystem.h"
#include "Engine/World.h"
#include "LevelTransitionActor.h"
#include "ItemManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SGameModeBase)

static TAutoConsoleVariable<bool> CVarSpawnBots(
    TEXT("game.SpawnBots"),
    true,
    TEXT("Enable spawning of bots via timer."),
    ECVF_Cheat
);

ASGameModeBase::ASGameModeBase()
{
    SpawnTimerInterval = 0.1f;
    CooldownTimeBetweenFailures = 0.0f;
    DesiredPowerupCount = 1;
    RequiredPowerupDistance = 2000;
    InitialSpawnCredit = 50;
    bAutoStartBotSpawning = false;
    bAutoRespawnPlayer = false;
    PlayerStateClass = ASPlayerState::StaticClass();
    CurrentLevel = 1;
    bIsTransitioning = false;

    AvailableSpawnCredit = InitialSpawnCredit;
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
    FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, TEXT("SaveGame"));
}

void ASGameModeBase::StartPlay()
{
    Super::StartPlay();

    AvailableSpawnCredit = InitialSpawnCredit;
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC && PC->GetPawn() == nullptr)
    {
        APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (NewPawn)
        {
            PC->Possess(NewPawn);
        }
    }
    if (CurrentLevel == 1)
    {
        SpawnInitialEnemies();
    }
}

void ASGameModeBase::OnLevelChanged()
{
    if (bIsTransitioning)
    {
        return;
    }

    bIsTransitioning = true;

    if (CurrentLevel == 7)
    {
        if (CompletedScreenWidgetClass)
        {
            CompletedScreenWidget = CreateWidget<UUserWidget>(GetWorld(), CompletedScreenWidgetClass);
            if (CompletedScreenWidget)
            {
                CompletedScreenWidget->AddToViewport();

                APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
                if (PC)
                {
                    FInputModeUIOnly InputMode;
                    InputMode.SetWidgetToFocus(CompletedScreenWidget->TakeWidget());
                    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                    PC->SetInputMode(InputMode);
                    PC->bShowMouseCursor = true;
                }
            }
        }
        return;
    }

    if (LoadingScreenWidgetClass)
    {
        LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenWidgetClass);
        if (LoadingScreenWidget)
        {
            LoadingScreenWidget->AddToViewport();
        }
    }

    FName OldLevelName = *FString::Printf(TEXT("Level%d"), CurrentLevel);
    FLatentActionInfo UnloadLatentInfo;
    UGameplayStatics::UnloadStreamLevel(this, OldLevelName, UnloadLatentInfo, false);

    CurrentLevel++;
    OnNewLevelStarted();

    FName NextLevelName = *FString::Printf(TEXT("Level%d"), CurrentLevel);
    LoadSubLevelAsync(NextLevelName);
}

void ASGameModeBase::OnSubLevelLoaded()
{
    GetWorldTimerManager().SetTimer(LevelLoadTimerHandle, FTimerDelegate::CreateLambda([this]()
        {
            FName LevelName = *FString::Printf(TEXT("Level%d"), CurrentLevel);
            ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(this, LevelName);
            if (StreamingLevel)
            {
                if (StreamingLevel->IsLevelLoaded() && StreamingLevel->IsLevelVisible())
                {
                    if (LoadingScreenWidget)
                    {
                        LoadingScreenWidget->RemoveFromParent();
                        LoadingScreenWidget = nullptr;
                    }

                    bIsTransitioning = false;

                    FString TagString = FString::Printf(TEXT("Start%d"), CurrentLevel);
                    TArray<AActor*> ActorsWithTag;
                    UGameplayStatics::GetAllActorsWithTag(this, FName(*TagString), ActorsWithTag);

                    SpawnInitialEnemies();

                    if (ActorsWithTag.Num() > 0)
                    {
                        AActor* SpawnActor = ActorsWithTag[0];
                        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
                        if (PC && PC->GetPawn() && SpawnActor)
                        {
                            PC->GetPawn()->SetActorLocation(SpawnActor->GetActorLocation());
                            PC->GetPawn()->SetActorRotation(SpawnActor->GetActorRotation());
                        }
                    }

                    GetWorldTimerManager().ClearTimer(LevelLoadTimerHandle);
                }
            }
        }), 0.5f, true);
}

void ASGameModeBase::LoadSubLevelAsync(FName LevelName)
{
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.ExecutionFunction = TEXT("OnSubLevelLoaded");
    LatentInfo.Linkage = 0;
    LatentInfo.UUID = 12345;

    UGameplayStatics::LoadStreamLevel(
        this,
        LevelName,
        true,    
        false,   
        LatentInfo
    );
}

void ASGameModeBase::OnNewLevelStarted()
{
    AvailableSpawnCredit = CalculateSpawnCredit(CurrentLevel);
}

void ASGameModeBase::UpdateSpawnRateForLevel()
{
    AvailableSpawnCredit = CalculateSpawnCredit(CurrentLevel);
}

float ASGameModeBase::CalculateSpawnCredit(int32 Level)
{
    if (Level == 1)
    {
        return 50.0f;
    }
    else if (Level == 2)
    {
        return 100.0f;
    }
    else if (Level >= 3 && Level <= 7)
    {
        return 100.0f + (Level - 2) * 100.0f;
    }
    else
    {
        return 0.0f;
    }
}

void ASGameModeBase::SpawnInitialEnemies()
{
    int32 NrOfAliveBots = 0;
    for (TActorIterator<AEnemyCharacter> It(GetWorld()); It; ++It)
    {
        if (USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(*It))
        {
            if (AttributeComp->IsAlive())
            {
                NrOfAliveBots++;
            }
        }
    }
    if (NrOfAliveBots == 0)
    {
        SpawnBotTimerElapsed();
    }
}

void ASGameModeBase::RequestPrimedActors()
{
    USActorPoolingSubsystem* PoolingSystem = GetWorld()->GetSubsystem<USActorPoolingSubsystem>();
    for (auto& Entry : ActorPoolClasses)
    {
        PoolingSystem->PrimeActorPool(Entry.Key, Entry.Value);
    }
}

void ASGameModeBase::StartSpawningBots()
{
    if (TimerHandle_SpawnBots.IsValid())
    {
        return;
    }
    GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
    if (UGameplayStatics::IsGamePaused(this) || LoadingScreenWidget)
    {
        return;
    }
    if (!CVarSpawnBots.GetValueOnGameThread())
    {
        return;
    }
    SelectedMonsterRow = nullptr;
    TArray<FEnemySpawnInfo*> Rows;
    if (MonsterTable)
    {
        MonsterTable->GetAllRows(TEXT(""), Rows);
    }
    if (Rows.Num() > 0)
    {
        SelectedMonsterRow = Rows[0];
    }
    int32 NrOfAliveBots = 0;
    for (TActorIterator<AEnemyCharacter> It(GetWorld()); It; ++It)
    {
        if (USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(*It))
        {
            if (AttributeComp->IsAlive())
            {
                NrOfAliveBots++;
            }
        }
    }
    if (NrOfAliveBots >= 10)
    {
        return;
    }
    if (!SpawnBotQuery)
    {
        return;
    }
    FEnvQueryRequest Request(SpawnBotQuery, this);
    Request.Execute(EEnvQueryRunMode::RandomBest5Pct, this, &ASGameModeBase::OnBotSpawnQueryCompleted);
}

void ASGameModeBase::OnBotSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result)
{
    if (!Result.IsValid() || !Result->IsSuccessful())
    {
        return;
    }

    TArray<FVector> Locations;
    Result->GetAllAsLocations(Locations);
    if (Locations.Num() == 0 || !SelectedMonsterRow || !MonsterTable)
    {
        return;
    }

    const float SpawnCost = SelectedMonsterRow->SpawnCost;
    for (int32 i = 0; i < Locations.Num(); i++)
    {
        if (AvailableSpawnCredit < SpawnCost)
        {
            break;
        }
        FVector SpawnLocation = Locations[i];
        SpawnScaledEnemy(SpawnLocation);
    }
}

void ASGameModeBase::SpawnScaledEnemy(const FVector& SpawnLocation)
{
    if (!SelectedMonsterRow)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnScaledEnemy: No monster row selected."));
        return;
    }

    USActorPoolingSubsystem* PoolingSystem = GetWorld()->GetSubsystem<USActorPoolingSubsystem>();
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AActor* NewEnemy = nullptr;
    if (PoolingSystem)
    {
        NewEnemy = PoolingSystem->AcquireFromPool(SelectedMonsterRow->MonsterClass, FTransform(SpawnLocation), SpawnParams);
    }
    if (!NewEnemy)
    {
        NewEnemy = GetWorld()->SpawnActor<AActor>(SelectedMonsterRow->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
    }

    if (NewEnemy)
    {
        AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(NewEnemy);
        if (EnemyCharacter)
        {
            if (USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(EnemyCharacter))
            {
                float BaseHealth = AttributeComp->GetHealthMax();
                float BaseDamage = EnemyCharacter->GetBaseDamage();

                int32 Level = CurrentLevel;
                float ScaledHealth = BaseHealth + (Level - 1) * 10.0f;
                float ScaledDamage = BaseDamage + (Level - 1) * 5.0f;

                float DeltaHealth = ScaledHealth - AttributeComp->GetHealthMax();
                AttributeComp->ApplyMaxHealthChange(DeltaHealth);
                AttributeComp->SetCurrentHealth(ScaledHealth);

                EnemyCharacter->SetDamage(ScaledDamage);

                UE_LOG(LogTemp, Log, TEXT("Spawned enemy at %s with Health: %.1f, Damage: %.1f (Level %d)"),
                    *SpawnLocation.ToString(), ScaledHealth, ScaledDamage, Level);
            }
        }
        AvailableSpawnCredit -= SelectedMonsterRow->SpawnCost;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnScaledEnemy: Failed to spawn enemy."));
    }
}

void ASGameModeBase::OnPowerupSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result)
{
    if (!Result.IsValid())
    {
        return;
    }
    FEnvQueryResult* QueryResult = Result.Get();
    if (!QueryResult->IsSuccessful())
    {
        return;
    }
    TArray<FVector> Locations;
    QueryResult->GetAllAsLocations(Locations);
    TArray<FVector> UsedLocations;
    int32 SpawnCounter = 0;
    while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
    {
        int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);
        FVector PickedLocation = Locations[RandomLocationIndex];
        Locations.RemoveAt(RandomLocationIndex);
        bool bValidLocation = true;
        for (const FVector& OtherLocation : UsedLocations)
        {
            if ((PickedLocation - OtherLocation).Size() < RequiredPowerupDistance)
            {
                bValidLocation = false;
                break;
            }
        }
        if (!bValidLocation)
        {
            continue;
        }
        int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
        TSubclassOf<AActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];
        GetWorld()->SpawnActor<AActor>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);
        UsedLocations.Add(PickedLocation);
        SpawnCounter++;
    }
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
    if (ensure(Controller))
    {
        Controller->UnPossess();
        RestartPlayer(Controller);
    }
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
    UE_LOG(LogTemp, Log, TEXT("OnActorKilled called for actor: %s"), *VictimActor->GetName());

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(VictimActor);
    if (Enemy)
    {
        APawn* KillerPawn = Cast<APawn>(Killer);
        if (KillerPawn)
        {
            ASPlayerState* PS = KillerPawn->GetPlayerState<ASPlayerState>();
            if (PS)
            {
                PS->AddCredits(25);
                UE_LOG(LogTemp, Log, TEXT("Added credits to killer's player state."));
            }
        }

        bool bAnyEnemyAlive = false;
        for (TActorIterator<AEnemyCharacter> It(GetWorld()); It; ++It)
        {
            if (USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(*It))
            {
                if (AttributeComp->IsAlive())
                {
                    bAnyEnemyAlive = true;
                    break;
                }
            }
        }
        UE_LOG(LogTemp, Log, TEXT("Remaining enemy alive: %s"), bAnyEnemyAlive ? TEXT("YES") : TEXT("NO"));

        if (!bAnyEnemyAlive)
        {
            UE_LOG(LogTemp, Log, TEXT("All enemies are dead. Preparing to spawn item."));

            FString SpawnTagStr = FString::Printf(TEXT("spawn_level%d"), CurrentLevel);
            FName SpawnTag(*SpawnTagStr);
            TArray<AActor*> SpawnPoints;
            UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpawnTag, SpawnPoints);
            UE_LOG(LogTemp, Log, TEXT("Found %d spawn points with tag %s."), SpawnPoints.Num(), *SpawnTagStr);

            if (SpawnPoints.Num() > 0)
            {
                int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
                AActor* SpawnPoint = SpawnPoints[RandomIndex];
                FVector SpawnLocation = SpawnPoint->GetActorLocation();
                FRotator SpawnRotation = SpawnPoint->GetActorRotation();
                UE_LOG(LogTemp, Log, TEXT("Using spawn point %s at location: %s"), *SpawnPoint->GetName(), *SpawnLocation.ToString());

                if (LevelTransitionActorClass)
                {
                    ALevelTransitionActor* TransitionActor = GetWorld()->SpawnActor<ALevelTransitionActor>(
                        LevelTransitionActorClass, SpawnLocation, SpawnRotation);
                    if (TransitionActor)
                    {
                        UE_LOG(LogTemp, Log, TEXT("Transition actor spawned at location: %s"), *TransitionActor->GetActorLocation().ToString());
                        if (!ItemManager)
                        {
                            for (TActorIterator<AItemManager> It(GetWorld()); It; ++It)
                            {
                                ItemManager = *It;
                                break;
                            }
                            if (!ItemManager && ItemManagerClass)
                            {
                                ItemManager = GetWorld()->SpawnActor<AItemManager>(ItemManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
                                if (ItemManager)
                                {
                                    UE_LOG(LogTemp, Log, TEXT("ItemManager spawned in OnActorKilled."));
                                }
                                else
                                {
                                    UE_LOG(LogTemp, Warning, TEXT("Failed to spawn ItemManager in OnActorKilled."));
                                }
                            }
                        }

                        if (ItemManager)
                        {
                            FVector LeftOffset = -TransitionActor->GetActorRightVector() * 500.f;
                            FVector ItemSpawnLocation = TransitionActor->GetActorLocation() + LeftOffset;
                            UE_LOG(LogTemp, Log, TEXT("Spawning item at location: %s"), *ItemSpawnLocation.ToString());
                            ItemManager->SpawnItemAtLocation(ItemSpawnLocation);
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("ItemManager pointer is still null in OnActorKilled!"));
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Transition Actor."));
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("LevelTransitionActorClass is not assigned."));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No spawn points found with tag %s."), *SpawnTagStr);
            }
        }
    }
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
    if (SG)
    {
        SG->HandleStartingNewPlayer(NewPlayer);
        if (SG->HasValidSave())
        {
            FTimerHandle TimerHandle;
            GetWorldTimerManager().SetTimer(TimerHandle, [SG, NewPlayer]()
                {
                    SG->OverrideSpawnTransform(NewPlayer);
                }, 1.0f, false);
        }
    }
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void ASGameModeBase::KillAll()
{
    for (TActorIterator<AEnemyCharacter> It(GetWorld()); It; ++It)
    {
        if (USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(*It))
        {
            if (AttributeComp->IsAlive())
            {
                AttributeComp->Kill(this);
            }
        }
    }
}
