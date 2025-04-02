#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CheckDistance.generated.h"

UCLASS(BlueprintType) // Add this line to expose the class to Blueprints
class DIVINE_API UBTTask_CheckDistance : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_CheckDistance();

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    FBlackboardKeySelector AttackRangeKey;

    UPROPERTY(EditAnywhere, Category = "AI")
    FBlackboardKeySelector TargetActorKey;

    UPROPERTY(EditAnywhere, Category = "AI")
    float MaxAttackRange = 130.0f;

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};