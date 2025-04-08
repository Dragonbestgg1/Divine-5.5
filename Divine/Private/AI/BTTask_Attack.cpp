#include "AI/BTTask_Attack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "AI/EnemyCharacter.h"
#include "Components/SkeletalMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTTask_Attack)

UBTTask_Attack::UBTTask_Attack()
{
	AttackRange = 130.0f;
	AttackDamage = 20.0f;
	TargetActorKey.SelectedKeyName = "TargetActor";
	SwordSocket = "SwordSocket";
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (OwnerComp.GetWorld()->IsPaused())
    {
        return EBTNodeResult::Failed;
    }

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor) return EBTNodeResult::Failed;

    const float Distance = FVector::Distance(Enemy->GetActorLocation(), TargetActor->GetActorLocation());
    if (Distance > AttackRange)
    {
        return EBTNodeResult::Failed;
    }

    Enemy->PlayAttackMontage();

    USkeletalMeshComponent* MeshComp = Enemy->GetMesh();
    if (!MeshComp) return EBTNodeResult::Failed;

    FVector SwordLocation = MeshComp->GetSocketLocation(SwordSocket);

    if (TargetActor->GetDistanceTo(Enemy) <= AttackRange)
    {
        UGameplayStatics::ApplyDamage(TargetActor, AttackDamage, AIController, Enemy, UDamageType::StaticClass());
    }

    return EBTNodeResult::Succeeded;
}
