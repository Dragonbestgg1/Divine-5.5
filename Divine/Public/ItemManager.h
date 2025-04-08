#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInfo.h"
#include "ItemManager.generated.h"

class AItemActor;

UCLASS()
class DIVINE_API AItemManager : public AActor
{
    GENERATED_BODY()

public:
    AItemManager();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
    class UDataTable* ItemDataTable;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
    TArray<FCustomItemInfo> AvailableItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
    TSubclassOf<AItemActor> ItemActorClass;

    UFUNCTION(BlueprintCallable, Category = "Items")
    void SpawnItemAtLocation(const FVector& SpawnLocation);

    UFUNCTION(BlueprintCallable, Category = "Items")
    void ResetItemPool();

protected:
    virtual void BeginPlay() override;

    void LoadItemsFromDataTable();

    FCustomItemInfo* SelectRandomItem();
};
