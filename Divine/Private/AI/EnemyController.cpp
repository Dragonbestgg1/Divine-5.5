#include "AI/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyController)


void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("Behavior Tree is nullptr! Please assign BehaviorTree in your AI Controller.")))
	{
		RunBehaviorTree(BehaviorTree);
	}
}

AEnemyController::AEnemyController()
{
	PrimaryActorTick.bTickEvenWhenPaused = false;
}