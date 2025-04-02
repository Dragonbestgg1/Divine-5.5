// ItemActor.h
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

    // Initialize the actor with the full item info from the data table.
    UFUNCTION(BlueprintCallable, Category = "Item")
    void Initialize(const FCustomItemInfo& NewItemInfo);

    // Called when the player picks up this item.
    UFUNCTION(BlueprintCallable, Category = "Item")
    void OnPickedUp(class ACharacter* PickingCharacter);

protected:
    // The item type.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    EItemType ItemType;

    // List of effects.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    TArray<FItemEffect> ItemEffects;

    // Mesh component to display the item model.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    class UStaticMeshComponent* MeshComp;

    // Light component to add a glow effect.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    class UPointLightComponent* GlowLight;

    // Store the full item information.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    FCustomItemInfo ItemInfo;

    // Overlap event handler.
    UFUNCTION()
    void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    // Override BeginPlay to check for overlaps.
    virtual void BeginPlay() override;
};
