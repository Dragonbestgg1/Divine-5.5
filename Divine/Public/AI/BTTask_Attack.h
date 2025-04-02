#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

UCLASS()
class DIVINE_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackRange = 130.0f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackDamage = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	FName SwordSocket;

public:

	UBTTask_Attack();
};
