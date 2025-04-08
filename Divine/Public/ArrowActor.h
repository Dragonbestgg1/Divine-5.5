#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArrowActor.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class DIVINE_API AArrowActor : public AActor
{
    GENERATED_BODY()

public:
    AArrowActor();

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProjectileMovementComponent* ProjectileMovementComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow")
    float DamageAmount;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

public:

    UFUNCTION(BlueprintCallable, Category = "Arrow")
    void SetDamageAmount(float InDamage);

    UFUNCTION(BlueprintCallable, Category = "Arrow")
    float GetDamageAmount() const;
};
