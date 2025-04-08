#include "PlayerDamageComponent.h"

UPlayerDamageComponent::UPlayerDamageComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    BaseDamage = 10.f;
    CollectedFlatBonus = 0.f;
    CollectedMultiplier = 1.f;
}

float UPlayerDamageComponent::GetActualDamage() const
{
    float Damage = (BaseDamage + CollectedFlatBonus) * CollectedMultiplier;

    for (const FConditionalDamageModifier& Modifier : ConditionalModifiers)
    {
        if (Modifier.EffectType == EConditionalDamageEffectType::ConditionalDamageMultiplierForDecreasedDamage)
        {
            if (Damage <= Modifier.ConditionThreshold)
            {
                Damage *= Modifier.MultiplierValue;
            }
        }
    }

    return Damage;
}

void UPlayerDamageComponent::AddFlatDamageBonus(float Bonus)
{
    CollectedFlatBonus += Bonus;
}

void UPlayerDamageComponent::AddDamageMultiplier(float Multiplier)
{
    CollectedMultiplier *= Multiplier;
}

void UPlayerDamageComponent::AddConditionalDamageMultiplier(float ConditionThreshold, float Multiplier)
{
    FConditionalDamageModifier NewModifier;
    NewModifier.EffectType = EConditionalDamageEffectType::ConditionalDamageMultiplierForDecreasedDamage;
    NewModifier.ConditionThreshold = ConditionThreshold;
    NewModifier.MultiplierValue = Multiplier;
    ConditionalModifiers.Add(NewModifier);
}
