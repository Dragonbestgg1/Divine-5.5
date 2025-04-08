#include "ItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Character.h"
#include "PlayerDamageComponent.h"
#include "SAttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "SPlayerState.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    MeshComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::HandleOverlap);

    GlowLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("GlowLight"));
    GlowLight->SetupAttachment(RootComponent);
    GlowLight->SetIntensity(5000.f);
    GlowLight->SetLightColor(FLinearColor::Yellow);
    GlowLight->SetVisibility(true);
}

void AItemActor::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> OverlappingActors;
    MeshComp->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());
    for (AActor* Actor : OverlappingActors)
    {
        if (ACharacter* Character = Cast<ACharacter>(Actor))
        {
            UE_LOG(LogTemp, Log, TEXT("AItemActor::BeginPlay found overlapping character: %s"), *Character->GetName());
            OnPickedUp(Character);
            break;
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
