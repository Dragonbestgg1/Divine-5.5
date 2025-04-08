#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemInfo.h"
#include "CustomItemRow.generated.h"

USTRUCT(BlueprintType)
struct FCustomItemRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TArray<FItemEffect> Effects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float RarityWeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemRarity Rarity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UStaticMesh* ItemMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 ScoreValue;
};
