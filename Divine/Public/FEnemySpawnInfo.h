// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FEnemySpawnInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FEnemySpawnInfo : public FTableRowBase
{
    GENERATED_BODY()

    FEnemySpawnInfo()
        : SpawnCost(0.0f) // Initialize SpawnCost to 0.0f or your chosen default
    {
    }

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
    TSubclassOf<AActor> MonsterClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
    float SpawnCost;
};
