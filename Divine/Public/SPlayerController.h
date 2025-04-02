#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerHUDWidget.h"  // Include our HUD widget header so UPlayerHUDWidget is defined.
#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);

UCLASS()
class DIVINE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

protected:
	// Pause menu widget variables
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> PauseMenuInstance;

	// Combined HUD widget for health and score.
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidgetInstance;

	// Functions

	UFUNCTION(BlueprintCallable, Category = "Pause")
	void PauseGame();

	UFUNCTION(BlueprintCallable, Category = "Pause")
	void UnpauseGame();

	virtual void SetupInputComponent() override;

	UPROPERTY(BlueprintAssignable)
	FOnPawnChanged OnPawnChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChanged OnPlayerStateReceived;

	virtual void SetPawn(APawn* InPawn) override;

	// Override BeginPlayingState to create our HUD widget
	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

	virtual void OnRep_PlayerState() override;

private:
	bool bIsUsingGamepad;
};
