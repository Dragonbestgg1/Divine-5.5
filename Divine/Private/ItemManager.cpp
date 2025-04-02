#include "ItemManager.h"
#include "ItemActor.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "CustomItemRow.h"  // Your DataTable row struct (updated to include ScoreValue)
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"

AItemManager::AItemManager()
{
    PrimaryActorTick.bCanEverTick = false;

    // Fallback: Try to assign ItemActorClass if not already assigned via Blueprint.
    static ConstructorHelpers::FClassFinder<AItemActor> ItemActorBPClass(TEXT("/Game/Divine/Items/Items"));
    if (ItemActorBPClass.Succeeded())
    {
        ItemActorClass = ItemActorBPClass.Class;
        UE_LOG(LogTemp, Log, TEXT("ItemActorClass set to: %s"), *ItemActorClass->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find BP_ItemActor. Please assign ItemActorClass in the editor."));
    }
}

void AItemManager::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("AItemManager::BeginPlay called."));
    LoadItemsFromDataTable();
    UE_LOG(LogTemp, Log, TEXT("AItemManager: Loaded %d items."), AvailableItems.Num());
}

void AItemManager::LoadItemsFromDataTable()
{
    if (!ItemDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataTable is not assigned in AItemManager!"));
        return;
    }

    TArray<FCustomItemRow*> AllRows;
    ItemDataTable->GetAllRows<FCustomItemRow>(TEXT("ItemManager"), AllRows);
    UE_LOG(LogTemp, Log, TEXT("AItemManager: Found %d rows in the DataTable."), AllRows.Num());

    for (FCustomItemRow* Row : AllRows)
    {
        if (Row)
        {
            FCustomItemInfo NewItem;
            NewItem.ItemType = Row->ItemType;
            NewItem.Effects = Row->Effects;
            NewItem.RarityWeight = Row->RarityWeight;
            NewItem.Rarity = Row->Rarity;
            NewItem.ItemMesh = Row->ItemMesh;
            // Assign the score value from the data table row.
            NewItem.ScoreValue = Row->ScoreValue;
            NewItem.bHasSpawned = false;
            AvailableItems.Add(NewItem);

            UE_LOG(LogTemp, Log, TEXT("Loaded item: Type %d, RarityWeight %f, Rarity %d, Mesh %s, ScoreValue %d"),
                (uint8)NewItem.ItemType, NewItem.RarityWeight, (uint8)NewItem.Rarity,
                NewItem.ItemMesh ? *NewItem.ItemMesh->GetName() : TEXT("None"),
                NewItem.ScoreValue);
        }
    }
}

FCustomItemInfo* AItemManager::SelectRandomItem()
{
    float TotalWeight = 0.f;
    for (const FCustomItemInfo& Item : AvailableItems)
    {
        if (!Item.bHasSpawned)
        {
            TotalWeight += GetAdjustedWeight(Item);
        }
    }

    if (TotalWeight <= 0.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("AItemManager::SelectRandomItem: Total weight is 0. No available items to spawn."));
        return nullptr;
    }

    float RandomValue = FMath::FRandRange(0.f, TotalWeight);
    UE_LOG(LogTemp, Log, TEXT("AItemManager::SelectRandomItem: TotalWeight = %f, RandomValue = %f"), TotalWeight, RandomValue);

    for (FCustomItemInfo& Item : AvailableItems)
    {
        if (!Item.bHasSpawned)
        {
            float AdjustedWeight = GetAdjustedWeight(Item);
            RandomValue -= AdjustedWeight;
            if (RandomValue <= 0.f)
            {
                UE_LOG(LogTemp, Log, TEXT("AItemManager::SelectRandomItem: Selected item with AdjustedWeight = %f"), AdjustedWeight);
                return &Item;
            }
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("AItemManager::SelectRandomItem: No item selected after iteration."));
    return nullptr;
}

void AItemManager::SpawnItemAtLocation(const FVector& SpawnLocation)
{
    UE_LOG(LogTemp, Log, TEXT("AItemManager::SpawnItemAtLocation called at location: %s"), *SpawnLocation.ToString());

    if (!ItemActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("AItemManager: ItemActorClass is not assigned!"));
        return;
    }

    FCustomItemInfo* SelectedItem = SelectRandomItem();
    if (!SelectedItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("AItemManager::SpawnItemAtLocation: No available item selected."));
        return;
    }

    FTransform SpawnTransform;
    SpawnTransform.SetLocation(SpawnLocation);
    SpawnTransform.SetRotation(FQuat::Identity);

    AItemActor* NewItem = GetWorld()->SpawnActor<AItemActor>(ItemActorClass, SpawnTransform);
    if (!NewItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("AItemManager::SpawnItemAtLocation: SpawnActor returned nullptr!"));
        return;
    }

    // Pass the full FCustomItemInfo struct to the Initialize function.
    NewItem->Initialize(*SelectedItem);
    SelectedItem->bHasSpawned = true;

    UE_LOG(LogTemp, Log, TEXT("AItemManager::SpawnItemAtLocation: Spawned item successfully."));
}

void AItemManager::ResetItemPool()
{
    for (FCustomItemInfo& Item : AvailableItems)
    {
        Item.bHasSpawned = false;
    }
    UE_LOG(LogTemp, Log, TEXT("AItemManager::ResetItemPool: Reset spawned flags for all items."));
}
