// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SActorPoolingSubsystem.h"

#include "Divine.h"
#include "SActorPoolingInterface.h"
#include "Logging/StructuredLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SActorPoolingSubsystem)

static TAutoConsoleVariable CVarActorPoolingEnabled(
    TEXT("game.ActorPooling"),
    true,
    TEXT("Enable actor pooling for selected objects."),
    ECVF_Default
);

AActor* USActorPoolingSubsystem::SpawnActorPooled(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod SpawnHandling)
{
    if (!WorldContextObject)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnActorPooled failed: WorldContextObject is NULL!"));
        return nullptr;
    }

    USActorPoolingSubsystem* PoolingSubsystem = WorldContextObject->GetWorld()->GetSubsystem<USActorPoolingSubsystem>();
    if (!PoolingSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnActorPooled failed: Pooling Subsystem is NULL!"));
        return nullptr;
    }

    return PoolingSubsystem->AcquireFromPool(ActorClass, SpawnTransform, FActorSpawnParameters());
}

bool USActorPoolingSubsystem::ReleaseToPool(AActor* Actor)
{
    if (IsPoolingEnabled())
    {
        USActorPoolingSubsystem* PoolingSubsystem = Actor->GetWorld()->GetSubsystem<USActorPoolingSubsystem>();
        return PoolingSubsystem->ReleaseToPool_Internal(Actor);
    }

    SCOPED_NAMED_EVENT(DestroyActorNoPool, FColor::Red);
    Actor->Destroy();
    return false;
}

AActor* USActorPoolingSubsystem::FindPooledActor(TSubclassOf<AActor> ActorClass)
{
    FActorPool* ActorPool = AvailableActorPool.Find(ActorClass);

    if (ActorPool && ActorPool->FreeActors.Num() > 0)
    {
        AActor* PooledActor = ActorPool->FreeActors.Pop();
        return PooledActor;
    }

    return nullptr;
}

AActor* USActorPoolingSubsystem::AcquireFromPool(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, FActorSpawnParameters SpawnParams)
{
    if (!ActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("AcquireFromPool failed: ActorClass is NULL!"));
        return nullptr;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("AcquireFromPool failed: GetWorld() returned NULL!"));
        return nullptr;
    }

    AActor* PooledActor = FindPooledActor(ActorClass);
    if (PooledActor)
    {
        UE_LOG(LogTemp, Log, TEXT("Reusing pooled actor: %s"), *PooledActor->GetName());
        return PooledActor;
    }

    UE_LOG(LogTemp, Log, TEXT("No pooled actors available. Spawning a new one: %s"), *ActorClass->GetName());
    AActor* NewActor = World->SpawnActor<AActor>(ActorClass, SpawnTransform, SpawnParams);

    if (!NewActor)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn new actor of class: %s"), *ActorClass->GetName());
    }

    return NewActor;
}

bool USActorPoolingSubsystem::IsPoolingEnabled() const
{
    return CVarActorPoolingEnabled.GetValueOnAnyThread();
}

void USActorPoolingSubsystem::PrimeActorPool(TSubclassOf<AActor> ActorClass, int32 Amount)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("PrimeActorPool failed: GetWorld() returned NULL!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Priming Pool for %s (%d)"), *GetNameSafe(ActorClass), Amount);

    for (int i = 0; i < Amount; ++i)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AActor* NewActor = World->SpawnActor<AActor>(ActorClass, FTransform::Identity, Params);

        if (NewActor)
        {
            ReleaseToPool(NewActor);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to prime actor of class: %s"), *GetNameSafe(ActorClass));
        }
    }
}

bool USActorPoolingSubsystem::ReleaseToPool_Internal(AActor* Actor)
{
    SCOPED_NAMED_EVENT(ReleaseActorToPool, FColor::White);
    check(IsValid(Actor));

    Actor->SetActorEnableCollision(false);
    Actor->SetActorHiddenInGame(true);

    Actor->RouteEndPlay(EEndPlayReason::Destroyed);

    ISActorPoolingInterface::Execute_PoolEndPlay(Actor);

    FActorPool* ActorPool = &AvailableActorPool.FindOrAdd(Actor->GetClass());
    ActorPool->FreeActors.Add(Actor);

    return true;
}

AActor* USActorPoolingSubsystem::AcquireFromPool_Internal(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, FActorSpawnParameters SpawnParams)
{
    SCOPED_NAMED_EVENT(AcquireActorFromPool, FColor::White);

    AActor* AcquiredActor = FindPooledActor(ActorClass);
    if (AcquiredActor)
    {
        AcquiredActor->SetActorTransform(SpawnTransform);
        AcquiredActor->SetActorEnableCollision(true);
        AcquiredActor->SetActorHiddenInGame(false);

        AcquiredActor->DispatchBeginPlay();
        ISActorPoolingInterface::Execute_PoolBeginPlay(AcquiredActor);

        return AcquiredActor;
    }

    UE_LOG(LogTemp, Log, TEXT("Actor Pool empty, spawning new Actor: %s"), *GetNameSafe(ActorClass));

    return GetWorld()->SpawnActor<AActor>(ActorClass, SpawnTransform, SpawnParams);
}
