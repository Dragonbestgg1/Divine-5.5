// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SignificanceManager.h"
#include "Components/ActorComponent.h"
#include "SSignificanceComponent.generated.h"


UENUM(BlueprintType)
enum class ESignificanceValue : uint8
{

	Hidden = 0,

	Lowest = 1,

	Medium = 2,

	Highest = 3,

	Invalid = UINT8_MAX
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPostSignificanceChanged, ESignificanceValue, Significance);

USTRUCT(BlueprintType)
struct FSignificanceDistance
{
	GENERATED_BODY()

	FSignificanceDistance()
	{
		Significance = ESignificanceValue::Highest;
		MaxDistance = 1000.f;
	}

	FSignificanceDistance(ESignificanceValue InSignificance, float InMaxDistance) :
		Significance(InSignificance),
		MaxDistance(InMaxDistance) {}

	UPROPERTY(EditAnywhere)
	ESignificanceValue Significance;

	UPROPERTY(EditAnywhere, meta = (UIMin = 1000.0, UIMax = 50000))
	float MaxDistance;

	float GetMaxDistSqrd() const { return MaxDistance * MaxDistance; }
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), HideCategories = ("Collision", "Tags", "ComponentTick", "ComponentReplication", "Cooking", "Activation", "AssetUserData"))
class DIVINE_API USSignificanceComponent : public UActorComponent
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UPROPERTY(EditAnywhere, Category = "Optimization")
	bool bManageSignificance;

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "MaxDistance"))
	TArray<FSignificanceDistance> Thresholds;

	UPROPERTY(BlueprintReadOnly, Category = "Optimization")
	ESignificanceValue CurrentSignificance;

protected:

	UPROPERTY(EditAnywhere, Category = "Optimization")
	bool bInsignificantWhenOwnerIsHidden;

	UPROPERTY(EditDefaultsOnly, Category = "Optimization")
	bool bWaitOneFrame;

	UPROPERTY(EditDefaultsOnly, Category = "Optimization")
	bool bManageOwnerParticleSignificance;

	bool bHasImplementedInterface;

	UFUNCTION()
	void RegisterWithManager();

	float CalcSignificance(USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& Viewpoint) const;

	void PostSignificanceUpdate(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal);

	float GetSignificanceByDistance(float DistanceSqrd) const;

	void UpdateParticleSignificance(float NewSignificance);

public:

	UPROPERTY(BlueprintAssignable)
	FOnPostSignificanceChanged OnSignificanceChanged;

	USSignificanceComponent();
};
