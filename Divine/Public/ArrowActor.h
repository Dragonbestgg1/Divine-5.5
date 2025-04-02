#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArrowActor.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

/**
 * A simple arrow actor that flies forward, deals damage on hit, and then destroys itself.
 */
UCLASS()
class DIVINE_API AArrowActor : public AActor
{
    GENERATED_BODY()

public:
    AArrowActor();

protected:

    // Mesh component for the arrow's visual representation.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

    // Projectile movement component to handle arrow flight.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProjectileMovementComponent* ProjectileMovementComp;

    // How much damage the arrow deals on hit.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow")
    float DamageAmount;

    // Called when the arrow hits another actor (blocking collision).
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

public:

    // Sets the arrow's damage amount (can be called from Blueprint).
    UFUNCTION(BlueprintCallable, Category = "Arrow")
    void SetDamageAmount(float InDamage);

    // Gets the arrow's current damage amount (for debugging or UI).
    UFUNCTION(BlueprintCallable, Category = "Arrow")
    float GetDamageAmount() const;
};
