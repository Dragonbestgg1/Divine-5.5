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
UCLASS()
class DIVINE_API USActorPoolingSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "Actor Pooling", meta = (WorldContext = "WorldContextObject"))
    AActor* SpawnActorPooled(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod SpawnHandling);

    UFUNCTION(BlueprintCallable, Category = "Actor Pooling")
    bool ReleaseToPool(AActor* Actor);

    AActor* AcquireFromPool(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, FActorSpawnParameters SpawnParams);

    bool IsPoolingEnabled() const;

    void PrimeActorPool(TSubclassOf<AActor> ActorClass, int32 Amount);

protected:

    AActor* FindPooledActor(TSubclassOf<AActor> ActorClass);

    AActor* AcquireFromPool_Internal(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, FActorSpawnParameters SpawnParams);

    bool ReleaseToPool_Internal(AActor* Actor);

protected:

    UPROPERTY()
    TMap<TSubclassOf<AActor>, FActorPool> AvailableActorPool;
};
