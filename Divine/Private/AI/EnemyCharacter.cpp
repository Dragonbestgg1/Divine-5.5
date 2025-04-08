
#include "AI/EnemyCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "SAttributeComponent.h"
#include "BrainComponent.h"
#include "SWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SActionComponent.h"
#include "Components/SSignificanceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SGameModeBase.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyCharacter)


AEnemyCharacter::AEnemyCharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));
	ActionComp = CreateDefaultSubobject<USActionComponent>(TEXT("ActionComp"));
	SigManComp = CreateDefaultSubobject<USSignificanceComponent>(TEXT("SigManComp"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	USkeletalMeshComponent* SkelMesh = GetMesh();
	SkelMesh->SetGenerateOverlapEvents(true);
	SkelMesh->bUpdateOverlapsOnAnimationFinalize = false;
	SkelMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	HitFlash_CustomPrimitiveIndex = 0;
	TargetActorKey = "TargetActor";
	DestructionDelay = 3.0f;

	BaseDamage = 20.0f;
}

void AEnemyCharacter::MulticastPawnSeen_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("MulticastPawnSeen called, but no widget is used."));
}

void AEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this, &AEnemyCharacter::OnHealthChanged);
	SigManComp->OnSignificanceChanged.AddDynamic(this, &AEnemyCharacter::OnSignificanceChanged);
}

void AEnemyCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		if (InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		GetMesh()->SetCustomPrimitiveDataFloat(HitFlash_CustomPrimitiveIndex, GetWorld()->TimeSeconds);

		if (NewHealth > 0.0f && HitReactMontage)
		{
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(HitReactMontage);
			}
		}

		if (NewHealth <= 0.0f)
		{
			AAIController* AIC = GetController<AAIController>();
			if (AIC && AIC->GetBrainComponent())
			{
				AIC->GetBrainComponent()->StopLogic("Killed");
			}

			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			FTimerHandle TimerHandle_Destroy;
			FTimerDelegate Delegate_Destroy;
			Delegate_Destroy.BindUFunction(this, FName("DestroyEnemyCharacter"));
			GetWorldTimerManager().SetTimer(TimerHandle_Destroy, Delegate_Destroy, DestructionDelay, false);
		}
	}
}


void AEnemyCharacter::DestroyEnemyCharacter()
{
	Destroy();
}

void AEnemyCharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = GetController<AAIController>();
	if (!AIC) return;

	AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);

	if (NewTarget && MeleeAttackMontage)
	{
		float DistanceTo = FVector::Distance(GetActorLocation(), NewTarget->GetActorLocation());
		if (DistanceTo <= 130.0f)
		{
			PlayAnimMontage(MeleeAttackMontage);
		}
	}
}



AActor* AEnemyCharacter::GetTargetActor() const
{
	AAIController* AIC = GetController<AAIController>();
	return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
}


void AEnemyCharacter::OnPawnSeen(APawn* Pawn)
{
	if (GetTargetActor() != Pawn)
	{
		SetTargetActor(Pawn);

		MulticastPawnSeen();
	}
}

void AEnemyCharacter::PlayAttackMontage()
{
	if (AttackMontage && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}
}

void AEnemyCharacter::OnSignificanceChanged(ESignificanceValue Significance)
{

	if (Significance <= ESignificanceValue::Medium)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_NavWalking);
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	const bool bHiddenSignificance = Significance == ESignificanceValue::Hidden;
	SetActorTickEnabled(!bHiddenSignificance);
	GetCharacterMovement()->SetComponentTickEnabled(!bHiddenSignificance);


	EVisibilityBasedAnimTickOption AnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	switch (Significance)
	{
	case ESignificanceValue::Highest:
		AnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		break;
	case ESignificanceValue::Medium:
		AnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
		break;
	case ESignificanceValue::Lowest:
	case ESignificanceValue::Hidden:
	case ESignificanceValue::Invalid:
		AnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	}

	GetMesh()->VisibilityBasedAnimTickOption = AnimTickOption;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();

	if (CurrentLocation.Z < -1000.0f)
	{
		ASGameModeBase* GM = Cast<ASGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		int32 CurrentLevel = 1;
		if (GM)
		{
			CurrentLevel = GM->GetCurrentLevel();
		}

		FString SpawnTagStr = FString::Printf(TEXT("spawn_level%d"), CurrentLevel);
		FName SpawnTag(*SpawnTagStr);

		TArray<AActor*> SpawnPoints;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpawnTag, SpawnPoints);

		if (SpawnPoints.Num() > 0)
		{
			FVector SpawnLocation = SpawnPoints[0]->GetActorLocation();
			SpawnLocation.Z = 250.0f;
			SetActorLocation(SpawnLocation);
			UE_LOG(LogTemp, Warning, TEXT("Enemy %s repositioned to spawn location %s"), *GetNameSafe(this), *SpawnLocation.ToString());
		}
		else
		{
			CurrentLocation.Z = 250.0f;
			SetActorLocation(CurrentLocation);
			UE_LOG(LogTemp, Warning, TEXT("Enemy %s repositioned to Z=1000 (no spawn point found)"), *GetNameSafe(this));
		}
	}
}