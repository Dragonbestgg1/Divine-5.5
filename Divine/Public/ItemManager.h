#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInfo.h"  // Contains FCustomItemInfo and the inline GetAdjustedWeight() function, plus related enums
#include "ItemManager.generated.h"

// Forward declaration of your item actor.
class AItemActor;

UCLASS()
class DIVINE_API AItemManager : public AActor
{
    GENERATED_BODY()

public:
    AItemManager();

    // DataTable asset containing item definitions.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
    class UDataTable* ItemDataTable;

    // Array to store runtime item info loaded from the DataTable.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
    TArray<FCustomItemInfo> AvailableItems;

    // The item actor class to spawn.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
    TSubclassOf<AItemActor> ItemActorClass;

    // Spawns an item at a given location.
    UFUNCTION(BlueprintCallable, Category = "Items")
    void SpawnItemAtLocation(const FVector& SpawnLocation);

    // Resets the spawned flag for all items (so that a new game can have all items available).
    UFUNCTION(BlueprintCallable, Category = "Items")
    void ResetItemPool();

protected:
    virtual void BeginPlay() override;

    // Loads item definitions from the DataTable.
    void LoadItemsFromDataTable();

    // Returns a pointer to a random unspawned item using weighted random selection.
    FCustomItemInfo* SelectRandomItem();
};
