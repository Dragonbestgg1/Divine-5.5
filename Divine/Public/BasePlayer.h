#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "PlayerDamageComponent.h"
#include "BasePlayer.generated.h"

// Forward declarations of classes
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class UAnimMontage;
class USAttributeComponent;
class UParticleSystem;
class USActionComponent;

UCLASS()
class DIVINE_API ABasePlayer : public ACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_LookMouse;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_Interact;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_PrimaryAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> Input_Pause;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComp;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	int32 HitFlash_CustomPrimitiveIndex;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USInteractionComponent> InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USAttributeComponent> AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USActionComponent> ActionComp;

	void Move(const FInputActionInstance& Instance);
	void LookMouse(const FInputActionValue& InputValue);
	void LookStick(const FInputActionValue& InputValue);
	void PrimaryAttack();
	void PrimaryInteract();
	void HandlePauseInput(const FInputActionInstance& Instance);
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	virtual void PostInitializeComponents() override;

	void FindCrosshairTarget();
	void CrosshairTraceComplete(const FTraceHandle& InTraceHandle, FTraceDatum& InTraceDatum);
	FTraceHandle TraceHandle;

	bool bHasPawnTarget;

	float DestructionDelay;

	UFUNCTION()
	void DestroyBasePlayerCharacter();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY()
	UUserWidget* PauseMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> DeathScreenWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UPlayerDamageComponent* DamageComp;

public:

	ABasePlayer();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death")
	UAnimMontage* DeathMontage;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Death")
	void BlueprintPlayDeathMontage();

	UFUNCTION(BlueprintCallable, Category = "Components")
	USAttributeComponent* GetAttributeComponent() const { return AttributeComp; }


};
