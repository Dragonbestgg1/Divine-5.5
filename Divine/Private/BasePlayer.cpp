#include "BasePlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h" 
#include "SInteractionComponent.h"
#include "SAttributeComponent.h"
#include "SActionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Divine.h"
#include "Logging/StructuredLog.h"
#include "SharedGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "SPlayerController.h"
#include "TimerManager.h"
#include "ArrowActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BasePlayer)

ABasePlayer::ABasePlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	SpringArmComp->SetUsingAbsoluteRotation(true);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComp"));

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));

	DamageComp = CreateDefaultSubobject<UPlayerDamageComponent>(TEXT("PlayerDamageComponent"));

	ActionComp = CreateDefaultSubobject<USActionComponent>(TEXT("ActionComp"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	GetMesh()->bUpdateOverlapsOnAnimationFinalize = false;

	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	HitFlash_CustomPrimitiveIndex = 0;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = false;


	DestructionDelay = 5.0f;
}


void ABasePlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ABasePlayer::OnHealthChanged);
}

void ABasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const APlayerController* PC = GetController<APlayerController>();
	const ULocalPlayer* LP = PC->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	Subsystem->AddMappingContext(DefaultInputMapping, 0);

	UEnhancedInputComponent* InputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	InputComp->BindAction(Input_Move, ETriggerEvent::Triggered, this, &ABasePlayer::Move);
	InputComp->BindAction(Input_Jump, ETriggerEvent::Triggered, this, &ABasePlayer::Jump);
	InputComp->BindAction(Input_Interact, ETriggerEvent::Triggered, this, &ABasePlayer::PrimaryInteract);
	InputComp->BindAction(Input_Pause, ETriggerEvent::Triggered, this, &ABasePlayer::HandlePauseInput);
	InputComp->BindAction(Input_LookMouse, ETriggerEvent::Triggered, this, &ABasePlayer::LookMouse);

	InputComp->BindAction(Input_PrimaryAttack, ETriggerEvent::Triggered, this, &ABasePlayer::PrimaryAttack);

}

void ABasePlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FindCrosshairTarget();
}


void ABasePlayer::FindCrosshairTarget()
{
	ASPlayerController* PC = GetController<ASPlayerController>();

	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const float AimAssistDistance = 5000.f;
	const FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * AimAssistDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FCollisionShape Shape;
	Shape.SetSphere(50.f);

	FTraceDelegate Delegate = FTraceDelegate::CreateUObject(this, &ABasePlayer::CrosshairTraceComplete);

	TraceHandle = GetWorld()->AsyncSweepByChannel(EAsyncTraceType::Single, EyeLocation, TraceEnd, FQuat::Identity, ECC_Pawn, Shape, Params, FCollisionResponseParams::DefaultResponseParam, &Delegate);
}


void ABasePlayer::CrosshairTraceComplete(const FTraceHandle& InTraceHandle, FTraceDatum& InTraceDatum)
{
	if (InTraceDatum.OutHits.IsValidIndex(0))
	{
		FHitResult Hit = InTraceDatum.OutHits[0];
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			bHasPawnTarget = Hit.IsValidBlockingHit() && HitActor->IsA(APawn::StaticClass());
		}
		else
		{
			bHasPawnTarget = false;
		}
	}
}


void ABasePlayer::Move(const FInputActionInstance& Instance)
{
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		return;
	}

	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	const FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();

	AddMovementInput(ControlRot.Vector(), AxisValue.Y);

	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, AxisValue.X);
}

void ABasePlayer::LookMouse(const FInputActionValue& InputValue)
{
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		return;
	}

	const FVector2D Value = InputValue.Get<FVector2D>();
	AddControllerYawInput(Value.X);
	AddControllerPitchInput(Value.Y);
}



void ABasePlayer::LookStick(const FInputActionValue& InputValue)
{
	FVector2D Value = InputValue.Get<FVector2D>();

	bool XNegative = Value.X < 0.f;
	bool YNegative = Value.Y < 0.f;

	static const float LookYawRate = 100.0f;
	static const float LookPitchRate = 50.0f;

	Value = Value * Value;

	if (XNegative)
	{
		Value.X *= -1.f;
	}
	if (YNegative)
	{
		Value.Y *= -1.f;
	}

	float RateMultiplier = 1.0f;
	if (bHasPawnTarget)
	{
		RateMultiplier = 0.5f;
	}

	AddControllerYawInput(Value.X * (LookYawRate * RateMultiplier) * GetWorld()->GetDeltaSeconds());
	AddControllerPitchInput(Value.Y * (LookPitchRate * RateMultiplier) * GetWorld()->GetDeltaSeconds());
}

void ABasePlayer::PrimaryAttack()
{
	ActionComp->StartActionByName(this, SharedGameplayTags::Action_PrimaryAttack);
}

void ABasePlayer::PrimaryInteract()
{
	InteractionComp->PrimaryInteract();
}

void ABasePlayer::HandlePauseInput(const FInputActionInstance& Instance)
{
	APlayerController* PC = GetController<APlayerController>();
	if (!PC)
	{
		return;
	}

	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);

		if (PauseMenuWidget)
		{
			PauseMenuWidget->RemoveFromParent();
			PauseMenuWidget = nullptr;
		}

		FInputModeGameOnly GameInput;
		PC->SetInputMode(GameInput);
		PC->bShowMouseCursor = false;

		this->EnableInput(PC);
	}
	else
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		if (PauseMenuWidgetClass)
		{
			PauseMenuWidget = CreateWidget<UUserWidget>(PC, PauseMenuWidgetClass);
			if (PauseMenuWidget)
			{
				PauseMenuWidget->AddToViewport();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to create PauseMenuWidget!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PauseMenuWidgetClass is not set!"));
		}

		FInputModeGameAndUI InputMode;
		if (PauseMenuWidget)
		{
			InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
		}
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;

		this->DisableInput(PC);
	}
}


float ABasePlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (AttributeComp)
	{
		AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);
	}

	return DamageAmount;
}


void ABasePlayer::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		GetMesh()->SetCustomPrimitiveDataFloat(HitFlash_CustomPrimitiveIndex, GetWorld()->TimeSeconds);
	}

	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = GetController<APlayerController>();
		if (PC)
		{
			DisableInput(PC);
			if (DeathMontage)
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					AnimInstance->Montage_Play(DeathMontage);
				}
			}

			UGameplayStatics::DeleteGameInSlot("SaveGame02", 0);

			if (DeathScreenWidgetClass)
			{
				UUserWidget* DeathScreen = CreateWidget<UUserWidget>(PC, DeathScreenWidgetClass);
				if (DeathScreen)
				{
					DeathScreen->AddToViewport();
					FInputModeUIOnly UIInput;
					UIInput.SetWidgetToFocus(DeathScreen->TakeWidget());
					PC->SetInputMode(UIInput);
					PC->bShowMouseCursor = true;
				}
			}
		}

		FTimerHandle TimerHandle_Destroy;
		FTimerDelegate Delegate_Destroy;
		Delegate_Destroy.BindUFunction(this, FName("DestroyBasePlayerCharacter"));
		GetWorldTimerManager().SetTimer(TimerHandle_Destroy, Delegate_Destroy, DestructionDelay, false);
	}
}

void ABasePlayer::DestroyBasePlayerCharacter()
{
	Destroy();
}