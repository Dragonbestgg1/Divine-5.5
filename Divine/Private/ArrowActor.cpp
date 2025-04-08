#include "ArrowActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

AArrowActor::AArrowActor()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    MeshComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    MeshComp->SetNotifyRigidBodyCollision(true);
    MeshComp->OnComponentHit.AddDynamic(this, &AArrowActor::OnHit);

    ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
    ProjectileMovementComp->bRotationFollowsVelocity = true;
    ProjectileMovementComp->bShouldBounce = false;
    ProjectileMovementComp->InitialSpeed = 2000.f;
    ProjectileMovementComp->MaxSpeed = 2000.f;

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
    if (OtherActor && OtherActor != this)
    {
        UGameplayStatics::ApplyDamage(OtherActor, DamageAmount,
            GetInstigatorController(), this, UDamageType::StaticClass());
    }

    Destroy();
}
