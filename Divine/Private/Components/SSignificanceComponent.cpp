#include "Components/SSignificanceComponent.h"
#include "Divine.h"
#include "NiagaraComponent.h"
#include "Significance.h"
#include "ParticleHelper.h"
#include "Particles/ParticleSystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SSignificanceComponent)

static float GForcedSignificance = -1;
static FAutoConsoleVariableRef CVarSignificanceManager_ForceSignificance(
	TEXT("SigMan.ForceSignificance"),
	GForcedSignificance,
	TEXT("Force significance on all managed objects. -1 is default, 0-4 is hidden, lowest, medium, highest.\n"),
	ECVF_Cheat
);


USSignificanceComponent::USSignificanceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bWaitOneFrame = true;
	bManageSignificance = true;
	bHasImplementedInterface = false;
	bInsignificantWhenOwnerIsHidden = true;
	bManageOwnerParticleSignificance = true;
	CurrentSignificance = ESignificanceValue::Invalid;

	Thresholds.Emplace(ESignificanceValue::Highest, 5000.f);
}


void USSignificanceComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bManageSignificance)
	{
		bHasImplementedInterface = GetOwner()->Implements<USignificance>();

		if (bWaitOneFrame)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &USSignificanceComponent::RegisterWithManager);
		}
		else
		{
			RegisterWithManager();
		}

		if (bManageOwnerParticleSignificance)
		{

			TArray<UParticleSystemComponent*> CascadeParticles;
			GetOwner()->GetComponents<UParticleSystemComponent>(CascadeParticles);

			for (UParticleSystemComponent* Comp : CascadeParticles)
			{
				Comp->SetManagingSignificance(true);
			}
		}
	}
}


void USSignificanceComponent::RegisterWithManager()
{
	if (USignificanceManager* SignificanceManager = USignificanceManager::Get(GetWorld()))
	{
		auto SignificanceFunc = [&](USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& Viewpoint) -> float
			{
				return CalcSignificance(ObjectInfo, Viewpoint);
			};

		auto PostSignificanceFunc = [&](USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal)
			{
				PostSignificanceUpdate(ObjectInfo, OldSignificance, Significance, bFinal);
			};

		FName Tag = GetOwner()->GetClass()->GetFName();
		SignificanceManager->RegisterObject(this, Tag, SignificanceFunc, USignificanceManager::EPostSignificanceType::Concurrent, PostSignificanceFunc);
	}
}


void USSignificanceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bManageSignificance)
	{
		if (USignificanceManager* SignificanceManager = USignificanceManager::Get(GetWorld()))
		{
			SignificanceManager->UnregisterObject(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}


float USSignificanceComponent::CalcSignificance(USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& Viewpoint) const
{
	if (GForcedSignificance >= 0.0f)
	{
		return GForcedSignificance;
	}

	if (bInsignificantWhenOwnerIsHidden && GetOwner()->IsHidden())
	{
		return static_cast<float>(ESignificanceValue::Hidden);
	}

	if (bHasImplementedInterface)
	{
		float Significance = 0.0f;
		if (ISignificance::Execute_CalcSignificanceOverride(GetOwner(), Significance))
		{
			return Significance;
		}
	}

	FVector OriginLocation = GetOwner()->GetActorLocation();
	if (bHasImplementedInterface)
	{
		FVector OverriddenOrigin = FVector::ZeroVector;
		if (ISignificance::Execute_GetSignificanceLocation(GetOwner(), Viewpoint, OverriddenOrigin))
		{
			OriginLocation = OverriddenOrigin;
		}
	}

	const float DistanceSqrd = (OriginLocation - Viewpoint.GetLocation()).SizeSquared();
	return GetSignificanceByDistance(DistanceSqrd);
}


void USSignificanceComponent::PostSignificanceUpdate(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal)
{

	if (Significance != OldSignificance || CurrentSignificance == ESignificanceValue::Invalid)
	{
		CurrentSignificance = static_cast<ESignificanceValue>(Significance);
		UE_LOGFMT(LogGame, Log, "Significance for {owner} changed to {significance}", GetNameSafe(GetOwner()), UEnum::GetValueAsString(CurrentSignificance));
		OnSignificanceChanged.Broadcast(CurrentSignificance);
	}

	if (!bManageOwnerParticleSignificance)
	{
		UpdateParticleSignificance(Significance);
	}
}


float USSignificanceComponent::GetSignificanceByDistance(float DistanceSqrd) const
{
	const int32 NumThresholds = Thresholds.Num();
	if (NumThresholds == 0)
	{
		UE_LOGFMT(LogGame, Warning, "SignificanceManager: No distance thresholds set in {owner}.", GetNameSafe(GetOwner()));
		return static_cast<float>(ESignificanceValue::Highest);
	}

	if (DistanceSqrd >= Thresholds[NumThresholds - 1].GetMaxDistSqrd())
	{
		return static_cast<float>(ESignificanceValue::Lowest);
	}

	for (int32 Idx = 0; Idx < NumThresholds; Idx++)
	{
		const FSignificanceDistance& Item = Thresholds[Idx];
		if (DistanceSqrd <= (Item.GetMaxDistSqrd()))
		{
			return static_cast<float>(Item.Significance);
		}
	}

	return static_cast<float>(ESignificanceValue::Highest);
}


void USSignificanceComponent::UpdateParticleSignificance(float NewSignificance)
{
	TArray<UNiagaraComponent*> NiagaraSystems;
	GetOwner()->GetComponents<UNiagaraComponent>(NiagaraSystems);

	for (UNiagaraComponent* Comp : NiagaraSystems)
	{

		Comp->SetSystemSignificanceIndex(NewSignificance);
	}

	{
		EParticleSignificanceLevel CurrSignificance;
		if (NewSignificance == static_cast<float>(ESignificanceValue::Highest))
		{
			CurrSignificance = EParticleSignificanceLevel::Low;
		}
		else if (NewSignificance <= static_cast<float>(ESignificanceValue::Lowest))
		{
			CurrSignificance = EParticleSignificanceLevel::Critical;
		}
		else
		{
			CurrSignificance = EParticleSignificanceLevel::Medium;
		}

		TArray<UParticleSystemComponent*> Particles;
		GetOwner()->GetComponents<UParticleSystemComponent>(Particles);

		for (UParticleSystemComponent* Comp : Particles)
		{
			Comp->SetRequiredSignificance(CurrSignificance);

		}
	}
}
