// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Significance.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USignificance : public UInterface
{
	GENERATED_BODY()
};

class DIVINE_API ISignificance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Significance")
	bool GetSignificanceLocation(FTransform Viewpoint, FVector& OutLocation);

	UFUNCTION(BlueprintNativeEvent, Category = "Significance")
	bool CalcSignificanceOverride(float& OutSignificance);
};
