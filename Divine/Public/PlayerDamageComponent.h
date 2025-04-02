#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerDamageComponent.generated.h"

UENUM(BlueprintType)
enum class EConditionalDamageEffectType : uint8
{
    None,
    // This effect multiplies damage if the current damage is below a threshold.
    ConditionalDamageMultiplierForDecreasedDamage
};

USTRUCT(BlueprintType)
struct FConditionalDamageModifier
{
    GENERATED_BODY()

    // For example, our only type right now.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    EConditionalDamageEffectType EffectType;

    // The threshold below which this modifier applies.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float ConditionThreshold;

    // The multiplier to apply when condition is met.
    // For example, if Value = 1.2 then damage is increased by 20%.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float MultiplierValue;

    FConditionalDamageModifier()
        : EffectType(EConditionalDamageEffectType::None)
        , ConditionThreshold(0.f)
        , MultiplierValue(1.f)
    {
    }
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DIVINE_API UPlayerDamageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPlayerDamageComponent();

    // Base damage (constant)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float BaseDamage;

    // Cumulative flat bonus damage.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
    float CollectedFlatBonus;

    // Cumulative multiplier (starts at 1.0).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
    float CollectedMultiplier;

    // Array of conditional modifiers.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
    TArray<FConditionalDamageModifier> ConditionalModifiers;

    // Returns actual damage using the formula:
    // (BaseDamage + CollectedFlatBonus) * CollectedMultiplier,
    // then applies any conditional modifiers if the damage is below a given threshold.
    UFUNCTION(BlueprintCallable, Category = "Damage")
    float GetActualDamage() const;

    // Adds a flat bonus to damage.
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void AddFlatDamageBonus(float Bonus);

    // Multiplies the damage multiplier.
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void AddDamageMultiplier(float Multiplier);

    // Adds a conditional damage multiplier.
    // If the computed damage is less than or equal to ConditionThreshold,
    // the damage will be multiplied by Multiplier.
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void AddConditionalDamageMultiplier(float ConditionThreshold, float Multiplier);
};
