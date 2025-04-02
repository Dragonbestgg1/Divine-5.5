#include "ArrowActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

AArrowActor::AArrowActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create the mesh component and set it as the root.
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    // Use a blocking collision profile so OnHit will fire instead of Overlap.
    MeshComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    // Enable 'Hit' events.
    MeshComp->SetNotifyRigidBodyCollision(true);
    MeshComp->OnComponentHit.AddDynamic(this, &AArrowActor::OnHit);

    // Create a projectile movement component to handle flight.
    ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
    ProjectileMovementComp->bRotationFollowsVelocity = true;
    ProjectileMovementComp->bShouldBounce = false;
    ProjectileMovementComp->InitialSpeed = 2000.f;
    ProjectileMovementComp->MaxSpeed = 2000.f;

    // Default damage.
    DamageAmount = 10.f;
}

void AArrowActor::SetDamageAmount(float InDamage)
{
    DamageAmount = InDamage;
}

float AArrowActor::GetDamageAmount() const
{
    return DamageAmount;
}

void AArrowActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    // Make sure we have a valid other actor (and we're not hitting ourselves).
    if (OtherActor && OtherActor != this)
    {
        // Apply damage once.
        UGameplayStatics::ApplyDamage(OtherActor, DamageAmount,
            GetInstigatorController(), this, UDamageType::StaticClass());
    }

    // Destroy the arrow so it doesn't remain in the world.
    Destroy();
}
