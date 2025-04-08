#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerHUDWidget.h"
#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);

UCLASS()
class DIVINE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> PauseMenuInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidgetInstance;

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

	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

	virtual void OnRep_PlayerState() override;

private:
	bool bIsUsingGamepad;
};
