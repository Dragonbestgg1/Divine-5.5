#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class USSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, class ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRecordTimeChanged, class ASPlayerState*, PlayerState, float, NewTime, float, OldRecord);

UCLASS()
class DIVINE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	// Constructor declaration
	ASPlayerState();

protected:
	// Our custom credits property will serve as the player's score.
	// (Score from APlayerState is not accessible because it's private.)
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_Credits, Category = "Credits")
	int32 Credits;

	// Existing property for record time
	UPROPERTY(BlueprintReadOnly)
	float PersonalRecordTime;

	// New Health property
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float PlayerHealth;

	// Function to handle replication of Credits
	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);

public:
	// Existing functions
	UFUNCTION(BlueprintCallable)
	bool UpdatePersonalRecord(float NewTime);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	int32 GetCredits() const;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void AddCredits(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool RemoveCredits(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void ResetCredits();

	// Delegates for credit and record changes.
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRecordTimeChanged OnRecordTimeChanged;

	// Timer handle to tick down the credits every second.
	UPROPERTY()
	FTimerHandle ScoreTickTimer;

	// Function called by the timer to decrement credits.
	UFUNCTION()
	void TickScoreDown();

	// Overrides
	virtual void BeginPlay() override;

	// Save/Load functions (note: no override specifier here)
	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);
	virtual void SavePlayerState_Implementation(USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);
	virtual void LoadPlayerState_Implementation(USSaveGame* SaveObject);

	// Replication setup.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
