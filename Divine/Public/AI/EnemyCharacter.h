// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SSignificanceComponent.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class USSignificanceComponent;
class UPawnSensingComponent;
class USAttributeComponent;
class UUserWidget;
class USWorldUserWidget;
class USActionComponent;

UCLASS()
class DIVINE_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TObjectPtr<USWorldUserWidget> ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	/* Widget to display when bot first sees a player. */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SpottedWidgetClass;

	/* Index must match the CustomPrimitiveData index used in the Overlay material */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	int32 HitFlash_CustomPrimitiveIndex;

	/* Key for AI Blackboard 'TargetActor' */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TargetActorKey;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetActor() const;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPawnSensingComponent> PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USAttributeComponent> AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USActionComponent> ActionComp;

	/* Handle fidelity for AI as they are off-screen or at far distances */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USSignificanceComponent> SigManComp;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UAnimMontage* MeleeAttackMontage;

	UFUNCTION()
	void OnSignificanceChanged(ESignificanceValue Significance);

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPawnSeen();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	float DestructionDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* HitReactMontage;


	UFUNCTION(BlueprintCallable, Category = "Death")
	void DestroyEnemyCharacter();

public:

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float BaseDamage;

	// Getter for the base damage.
	float GetBaseDamage() const { return BaseDamage; }

	// Setter for updating the damage (e.g., after scaling).
	void SetDamage(float NewDamage) { BaseDamage = NewDamage; }

	// In EnemyCharacter.h
	virtual void Tick(float DeltaTime) override;

	void PlayAttackMontage();

	AEnemyCharacter();

};
