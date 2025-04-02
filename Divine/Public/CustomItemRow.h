#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemInfo.h"  // Assuming this file declares EItemType, EItemRarity, and FItemEffect.
#include "CustomItemRow.generated.h"

USTRUCT(BlueprintType)
struct FCustomItemRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    // The type of the item.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemType ItemType;

    // Array of effects for this item.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TArray<FItemEffect> Effects;

    // Base weight used for weighted random selection.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float RarityWeight;

    // Rarity of the item.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemRarity Rarity;

    // The mesh/model for this item.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UStaticMesh* ItemMesh;

    // Score value for this item – set in the data table.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 ScoreValue;
};
