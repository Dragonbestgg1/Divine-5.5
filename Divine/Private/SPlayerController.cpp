#include "SPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerHUDWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SPlayerController)

void ASPlayerController::PauseGame()
{
    if (!UGameplayStatics::IsGamePaused(GetWorld()))
    {
        UGameplayStatics::SetGamePaused(GetWorld(), true);

        PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
        if (PauseMenuInstance)
        {
            PauseMenuInstance->AddToViewport(100);
            bShowMouseCursor = true;

            FInputModeUIOnly UIInput;
            UIInput.SetWidgetToFocus(PauseMenuInstance->TakeWidget());
            SetInputMode(UIInput);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create PauseMenuInstance!"));
        }

        if (APawn* ControlledPawn = GetPawn())
        {
            ControlledPawn->DisableInput(this);
        }
    }
}

void ASPlayerController::UnpauseGame()
{
    if (UGameplayStatics::IsGamePaused(GetWorld()))
    {
        UGameplayStatics::SetGamePaused(GetWorld(), false);

        if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
        {
            PauseMenuInstance->RemoveFromParent();
            PauseMenuInstance = nullptr;
        }

        FInputModeGameOnly GameInput;
        SetInputMode(GameInput);
        bShowMouseCursor = false;

        if (APawn* ControlledPawn = GetPawn())
        {
            ControlledPawn->EnableInput(this);
        }
    }
}

void ASPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("PauseMenu", IE_Pressed, this, &ASPlayerController::PauseGame);

    InputComponent->BindAction("Return", IE_Pressed, this, &ASPlayerController::UnpauseGame);
}

void ASPlayerController::SetPawn(APawn* InPawn)
{
    Super::SetPawn(InPawn);
    OnPawnChanged.Broadcast(InPawn);
}

void ASPlayerController::BeginPlayingState()
{
    Super::BeginPlayingState();

    if (PlayerHUDWidgetClass)
    {
        PlayerHUDWidgetInstance = CreateWidget<UPlayerHUDWidget>(this, PlayerHUDWidgetClass);
        if (PlayerHUDWidgetInstance)
        {
            PlayerHUDWidgetInstance->AddToViewport();
        }
    }

    BlueprintBeginPlayingState();
}

void ASPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    OnPlayerStateReceived.Broadcast(PlayerState);
}
