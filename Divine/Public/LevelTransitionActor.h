#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionActor.generated.h"

UCLASS()
class DIVINE_API ALevelTransitionActor : public AActor
{
    GENERATED_BODY()

public:
    ALevelTransitionActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
