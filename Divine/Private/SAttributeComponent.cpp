#include "SAttributeComponent.h"
#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "BasePlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SAttributeComponent)

static TAutoConsoleVariable<float> CVarDamageMultiplier(
	TEXT("game.DamageMultiplier"),
	1.0f,
	TEXT("Global Damage Modifier for Attribute Component."),
	ECVF_Cheat
);

USAttributeComponent::USAttributeComponent()
{
	HealthMax = 100.f;
	Health = HealthMax;
	SetIsReplicatedByDefault(true);
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged() && Delta < 0.f)
	{
		return false;
	}

	if (Delta < 0.f)
	{
		const float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= DamageMultiplier;
	}

	float OldHealth = Health;
	float NewHealth = FMath::Clamp(Health + Delta, 0.f, HealthMax);
	float ActualDelta = NewHealth - OldHealth;

	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;
		if (!FMath::IsNearlyZero(ActualDelta))
		{
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
		}

		if (ActualDelta < 0.f && FMath::IsNearlyZero(Health))
		{
			ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
			if (GM)
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	return !FMath::IsNearlyZero(ActualDelta);
}

void USAttributeComponent::ApplyMaxHealthChange(float DeltaMax)
{
	if (!GetOwner()->IsA(ABasePlayer::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyMaxHealthChange called on non-player actor; enemy scaling is handled elsewhere."));
		return;
	}

	float OldMax = HealthMax;
	HealthMax = FMath::Max(HealthMax + DeltaMax, 1.f);

	if (DeltaMax > 0.f)
	{
		Health = HealthMax;
	}
	else
	{
		Health = FMath::Clamp(Health, 0.f, HealthMax);
	}

	OnHealthChanged.Broadcast(nullptr, this, Health, Health - OldMax);
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return FromActor->FindComponentByClass<USAttributeComponent>();
	}
	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}
	return false;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return FMath::IsNearlyEqual(Health, HealthMax);
}

float USAttributeComponent::GetHealth() const
{
	return Health;
}

float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

void USAttributeComponent::SetCurrentHealth(float NewHealth)
{
	Health = NewHealth;
	OnHealthChanged.Broadcast(nullptr, this, Health, 0.f);
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, HealthMax);
}
