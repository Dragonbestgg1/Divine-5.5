// ItemInfo.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "ItemInfo.generated.h"

//
// Enums for item types, effects, and rarity
//
UENUM(BlueprintType)
enum class EItemEffectType : uint8 {
    BonusDamage                             UMETA(DisplayName = "Bonus Damage"),
    BonusHealth                             UMETA(DisplayName = "Bonus Health"),
    DamageMultiplier                        UMETA(DisplayName = "Damage Multiplier"),
    ConditionalBonusDamage                  UMETA(DisplayName = "Conditional Bonus Damage"),
    ConditionalBonusHealthForDecreasedDmg   UMETA(DisplayName = "Conditional Bonus Health for Decreased Damage"),
    ConditionalBonusHealthForDecreasedMult    UMETA(DisplayName = "Conditional Bonus Health for Decreased Multiplier"),
    ConditionalDamageMultiplierForDecreasedDamage UMETA(DisplayName = "Conditional Damage Multiplier For Decreased Damage")
};

UENUM(BlueprintType)
enum class EItemRarity : uint8 {
    Common      UMETA(DisplayName = "Common"),
    Uncommon    UMETA(DisplayName = "Uncommon"),
    Rare        UMETA(DisplayName = "Rare"),
    Legendary   UMETA(DisplayName = "Legendary")
};

UENUM(BlueprintType)
enum class EItemType : uint8 {
    BonusDamage       UMETA(DisplayName = "Bonus Damage"),
    BonusHealth       UMETA(DisplayName = "Bonus Health"),
    DamageMultiplier  UMETA(DisplayName = "Damage Multiplier")
};

//
// Struct representing a single item effect
//
USTRUCT(BlueprintType)
struct FItemEffect {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Effect")
    EItemEffectType EffectType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Effect")
    float Value;

    // For conditional effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Effect")
    float ConditionThreshold;

    FItemEffect()
        : EffectType(EItemEffectType::BonusDamage)
        , Value(0.f)
        , ConditionThreshold(1.f)
    {
    }
};

//
// Struct holding full item data (used at runtime)
//
USTRUCT(BlueprintType)
struct FCustomItemInfo {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TArray<FItemEffect> Effects;

    // Base spawn weight that can be modified by rarity
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float RarityWeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemRarity Rarity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UStaticMesh* ItemMesh;

    // Score value for the item, set in the data table.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 ScoreValue;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    bool bHasSpawned;

    // Note: The initializer list order now matches the member declaration order.
    FCustomItemInfo()
        : ItemType(EItemType::BonusDamage)
        , Effects() // default initialization of array
        , RarityWeight(1.f)
        , Rarity(EItemRarity::Common)
        , ItemMesh(nullptr)
        , ScoreValue(0)
        , bHasSpawned(false)
    {
    }
};

//
// Inline helper to adjust weight based on rarity
//
inline float GetAdjustedWeight(const FCustomItemInfo& Item)
{
    float RarityMultiplier = 1.0f;
    switch (Item.Rarity)
    {
    case EItemRarity::Common:
        RarityMultiplier = 1.0f;
        break;
    case EItemRarity::Uncommon:
        RarityMultiplier = 0.8f;
        break;
    case EItemRarity::Rare:
        RarityMultiplier = 0.5f;
        break;
    case EItemRarity::Legendary:
        RarityMultiplier = 0.3f;
        break;
    default:
        break;
    }
    return Item.RarityWeight * RarityMultiplier;
}
