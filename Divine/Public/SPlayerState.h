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
	ASPlayerState();

protected:
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_Credits, Category = "Credits")
	int32 Credits;

	UPROPERTY(BlueprintReadOnly)
	float PersonalRecordTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float PlayerHealth;

	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);

public:
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

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRecordTimeChanged OnRecordTimeChanged;

	UPROPERTY()
	FTimerHandle ScoreTickTimer;

	UFUNCTION()
	void TickScoreDown();

	virtual void BeginPlay() override;
	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);
	virtual void SavePlayerState_Implementation(USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);
	virtual void LoadPlayerState_Implementation(USSaveGame* SaveObject);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
