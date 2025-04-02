// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SActorPoolingInterface.generated.h"

UINTERFACE(MinimalAPI)
class USActorPoolingInterface : public UInterface
{
	GENERATED_BODY()
};

class DIVINE_API ISActorPoolingInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void PoolBeginPlay();

	UFUNCTION(BlueprintNativeEvent)
	void PoolEndPlay();
};
