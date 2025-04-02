// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SActorPoolingSubsystem.generated.h"

USTRUCT()
struct FActorPool
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<TObjectPtr<AActor>> FreeActors;
};

/**
 * Actor Pooling Subsystem
 */
UCLASS()
class DIVINE_API USActorPoolingSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:

    /** Retrieves an actor from the pool or spawns a new one */
    UFUNCTION(BlueprintCallable, Category = "Actor Pooling", meta = (WorldContext = "WorldContextObject"))
    AActor* SpawnActorPooled(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod SpawnHandling);

    /** Releases an actor back into the pool */
    UFUNCTION(BlueprintCallable, Category = "Actor Pooling")
    bool ReleaseToPool(AActor* Actor);

    /** Retrieves an actor from the pool, or spawns a new one if none are available */
    AActor* AcquireFromPool(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, FActorSpawnParameters SpawnParams);

    /** Determines whether pooling is enabled */
    bool IsPoolingEnabled() const;

    /** Primes the actor pool with a specific amount of actors */
    void PrimeActorPool(TSubclassOf<AActor> ActorClass, int32 Amount);

protected:

    /** Finds a pooled actor that can be reused */
    AActor* FindPooledActor(TSubclassOf<AActor> ActorClass);

    /** Internal function to acquire an actor from the pool */
    AActor* AcquireFromPool_Internal(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, FActorSpawnParameters SpawnParams);

    /** Internal function to release an actor into the pool */
    bool ReleaseToPool_Internal(AActor* Actor);

protected:

    /** Map of available pooled actors by class */
    UPROPERTY()
    TMap<TSubclassOf<AActor>, FActorPool> AvailableActorPool;
};
