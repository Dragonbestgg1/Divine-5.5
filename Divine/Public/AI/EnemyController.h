#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

class UBehaviorTree;

UCLASS()
class DIVINE_API AEnemyController : public AAIController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	virtual void BeginPlay() override;

public:
	AEnemyController();
};
