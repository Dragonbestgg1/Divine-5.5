#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerDamageComponent.generated.h"

UENUM(BlueprintType)
enum class EConditionalDamageEffectType : uint8
{
    None,
    ConditionalDamageMultiplierForDecreasedDamage
};

USTRUCT(BlueprintType)
struct FConditionalDamageModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    EConditionalDamageEffectType EffectType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float ConditionThreshold;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float BaseDamage;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
    float CollectedFlatBonus;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
    float CollectedMultiplier;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
    TArray<FConditionalDamageModifier> ConditionalModifiers;

    UFUNCTION(BlueprintCallable, Category = "Damage")
    float GetActualDamage() const;

    UFUNCTION(BlueprintCallable, Category = "Damage")
    void AddFlatDamageBonus(float Bonus);

    UFUNCTION(BlueprintCallable, Category = "Damage")
    void AddDamageMultiplier(float Multiplier);

    UFUNCTION(BlueprintCallable, Category = "Damage")
    void AddConditionalDamageMultiplier(float ConditionThreshold, float Multiplier);
};
