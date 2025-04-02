// Fill out your copyright notice in the Description page of Project Settings.


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
	// Since we no longer use the world widget for enemy pawns, simply log the call.
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

		// Play hit reaction animation if enemy is hit but not killed.
		if (NewHealth > 0.0f && HitReactMontage)
		{
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(HitReactMontage);
			}
		}

		// Died
		if (NewHealth <= 0.0f)
		{
			// Stop behavior tree logic.
			AAIController* AIC = GetController<AAIController>();
			if (AIC && AIC->GetBrainComponent())
			{
				AIC->GetBrainComponent()->StopLogic("Killed");
			}

			// Switch to ragdoll.
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			// Optionally, schedule actor destruction.
			FTimerHandle TimerHandle_Destroy;
			FTimerDelegate Delegate_Destroy;
			Delegate_Destroy.BindUFunction(this, FName("DestroyEnemyCharacter"));
			GetWorldTimerManager().SetTimer(TimerHandle_Destroy, Delegate_Destroy, DestructionDelay, false);
		}
	}
}


void AEnemyCharacter::DestroyEnemyCharacter()
{
	Destroy(); // Actually destroy the actor
}

void AEnemyCharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = GetController<AAIController>();
	if (!AIC) return;

	AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);

	// Start attacking if within range
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

	// Check if the enemy has fallen below the threshold.
	if (CurrentLocation.Z < -1000.0f)
	{
		// Retrieve current level from the game mode using the getter.
		ASGameModeBase* GM = Cast<ASGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		int32 CurrentLevel = 1;
		if (GM)
		{
			CurrentLevel = GM->GetCurrentLevel();
		}

		// Build the dynamic tag, e.g. "spawn_level1"
		FString SpawnTagStr = FString::Printf(TEXT("spawn_level%d"), CurrentLevel);
		FName SpawnTag(*SpawnTagStr);

		// Find spawn points with that tag.
		TArray<AActor*> SpawnPoints;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpawnTag, SpawnPoints);

		if (SpawnPoints.Num() > 0)
		{
			FVector SpawnLocation = SpawnPoints[0]->GetActorLocation();
			// Use the spawn point's X and Y, but set Z to 1000.
			SpawnLocation.Z = 250.0f;
			SetActorLocation(SpawnLocation);
			UE_LOG(LogTemp, Warning, TEXT("Enemy %s repositioned to spawn location %s"), *GetNameSafe(this), *SpawnLocation.ToString());
		}
		else
		{
			// Fallback: just set Z to 1000, keep current X and Y.
			CurrentLocation.Z = 250.0f;
			SetActorLocation(CurrentLocation);
			UE_LOG(LogTemp, Warning, TEXT("Enemy %s repositioned to Z=1000 (no spawn point found)"), *GetNameSafe(this));
		}
	}
}