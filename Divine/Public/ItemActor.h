#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInfo.h"
#include "ItemActor.generated.h"

UCLASS()
class DIVINE_API AItemActor : public AActor
{
    GENERATED_BODY()

public:
    AItemActor();

    UFUNCTION(BlueprintCallable, Category = "Item")
    void Initialize(const FCustomItemInfo& NewItemInfo);

    UFUNCTION(BlueprintCallable, Category = "Item")
    void OnPickedUp(class ACharacter* PickingCharacter);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    EItemType ItemType;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    TArray<FItemEffect> ItemEffects;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    class UStaticMeshComponent* MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    class UPointLightComponent* GlowLight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    FCustomItemInfo ItemInfo;

    UFUNCTION()
    void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    virtual void BeginPlay() override;
};
