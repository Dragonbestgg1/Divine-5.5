#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "ItemInfo.generated.h"

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

USTRUCT(BlueprintType)
struct FItemEffect {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Effect")
    EItemEffectType EffectType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Effect")
    float Value;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Effect")
    float ConditionThreshold;

    FItemEffect()
        : EffectType(EItemEffectType::BonusDamage)
        , Value(0.f)
        , ConditionThreshold(1.f)
    {
    }
};

USTRUCT(BlueprintType)
struct FCustomItemInfo {
    GENERATED_BODY()

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    bool bHasSpawned;

    FCustomItemInfo()
        : ItemType(EItemType::BonusDamage)
        , Effects()
        , RarityWeight(1.f)
        , Rarity(EItemRarity::Common)
        , ItemMesh(nullptr)
        , ScoreValue(0)
        , bHasSpawned(false)
    {
    }
};

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
