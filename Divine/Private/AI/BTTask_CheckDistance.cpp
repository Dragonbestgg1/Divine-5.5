#include "AI/BTTask_CheckDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"

UBTTask_CheckDistance::UBTTask_CheckDistance()
{
    bNotifyTick = true; // Enable ticking
}

void UBTTask_CheckDistance::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    APawn* AICharacter = AIController->GetPawn();
    if (!AICharacter) return;

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

    if (!TargetActor) return;

    float Distance = FVector::Dist(AICharacter->GetActorLocation(), TargetActor->GetActorLocation());

    bool bInAttackRange = Distance <= MaxAttackRange;

    BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, bInAttackRange);

    if (OwnerComp.GetWorld()->IsPaused())
    {
        // Do nothing if paused
        return;
    }
}

EBTNodeResult::Type UBTTask_CheckDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Task execution logic can go here.
    return EBTNodeResult::InProgress; // Keep the task in progress to allow ticking
}
