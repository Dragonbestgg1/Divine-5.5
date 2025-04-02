#include "ItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Character.h"
#include "PlayerDamageComponent.h"  // For damage effects, if needed
#include "SAttributeComponent.h"    // For health effects
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "SPlayerState.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create and set up the mesh component.
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    // Set collision to QueryOnly and respond with overlap.
    MeshComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::HandleOverlap);

    // Create a point light component for glow.
    GlowLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("GlowLight"));
    GlowLight->SetupAttachment(RootComponent);
    GlowLight->SetIntensity(5000.f);
    GlowLight->SetLightColor(FLinearColor::Yellow);
    GlowLight->SetVisibility(true);
}

void AItemActor::BeginPlay()
{
    Super::BeginPlay();

    // Check if any character is already overlapping this item.
    TArray<AActor*> OverlappingActors;
    MeshComp->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());
    for (AActor* Actor : OverlappingActors)
    {
        if (ACharacter* Character = Cast<ACharacter>(Actor))
        {
            UE_LOG(LogTemp, Log, TEXT("AItemActor::BeginPlay found overlapping character: %s"), *Character->GetName());
            OnPickedUp(Character);
            break; // Only pick up once.
        }
    }
}

void AItemActor::Initialize(const FCustomItemInfo& NewItemInfo)
{
    ItemInfo = NewItemInfo;
    ItemType = NewItemInfo.ItemType;
    ItemEffects = NewItemInfo.Effects;
    if (NewItemInfo.ItemMesh)
    {
        MeshComp->SetStaticMesh(NewItemInfo.ItemMesh);
    }
    UE_LOG(LogTemp, Log, TEXT("AItemActor::Initialize called. ItemType: %d, Mesh: %s"), (uint8)ItemType, NewItemInfo.ItemMesh ? *NewItemInfo.ItemMesh->GetName() : TEXT("None"));
}

void AItemActor::OnPickedUp(ACharacter* PickingCharacter)
{
    UE_LOG(LogTemp, Log, TEXT("AItemActor::OnPickedUp called."));
    if (!PickingCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("AItemActor::OnPickedUp: PickingCharacter is null."));
        return;
    }

    // Get components from the character.
    USAttributeComponent* AttrComp = PickingCharacter->FindComponentByClass<USAttributeComponent>();
    UPlayerDamageComponent* DamageComp = PickingCharacter->FindComponentByClass<UPlayerDamageComponent>();

    for (const FItemEffect& Effect : ItemEffects)
    {
        switch (Effect.EffectType)
        {
        case EItemEffectType::BonusHealth:
            if (AttrComp)
            {
                UE_LOG(LogTemp, Log, TEXT("Applying BonusHealth: %f"), Effect.Value);
                AttrComp->ApplyMaxHealthChange(Effect.Value);
            }
            break;

        case EItemEffectType::BonusDamage:
            if (DamageComp)
            {
                UE_LOG(LogTemp, Log, TEXT("Applying BonusDamage: %f"), Effect.Value);
                DamageComp->AddFlatDamageBonus(Effect.Value);
            }
            break;

        case EItemEffectType::DamageMultiplier:
            if (DamageComp)
            {
                UE_LOG(LogTemp, Log, TEXT("Applying DamageMultiplier: %f"), Effect.Value);
                DamageComp->AddDamageMultiplier(Effect.Value);
            }
            break;

        case EItemEffectType::ConditionalDamageMultiplierForDecreasedDamage:
            if (DamageComp)
            {
                UE_LOG(LogTemp, Log, TEXT("Applying ConditionalDamageMultiplierForDecreasedDamage: Threshold=%f, Multiplier=%f"),
                    Effect.ConditionThreshold, Effect.Value);
                DamageComp->AddConditionalDamageMultiplier(Effect.ConditionThreshold, Effect.Value);
            }
            break;

        default:
            UE_LOG(LogTemp, Log, TEXT("AItemActor::OnPickedUp: Effect type %d not handled."), (uint8)Effect.EffectType);
            break;
        }
    }

    // Award score based on the data table value.
    if (ASPlayerState* PS = PickingCharacter->GetPlayerState<ASPlayerState>())
    {
        PS->AddCredits(ItemInfo.ScoreValue);
        UE_LOG(LogTemp, Log, TEXT("Added %d score for item pickup."), ItemInfo.ScoreValue);
    }

    UE_LOG(LogTemp, Log, TEXT("AItemActor::OnPickedUp: Pickup complete. Destroying actor."));
    Destroy();
}

void AItemActor::HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (ACharacter* Character = Cast<ACharacter>(OtherActor))
    {
        UE_LOG(LogTemp, Log, TEXT("AItemActor::HandleOverlap: Overlap detected with %s"), *Character->GetName());
        OnPickedUp(Character);
    }
}
