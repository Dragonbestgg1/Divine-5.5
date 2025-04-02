#include "SPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerHUDWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SPlayerController)

void ASPlayerController::PauseGame()
{
    // Only pause if the game is not already paused.
    if (!UGameplayStatics::IsGamePaused(GetWorld()))
    {
        // Pause the game
        UGameplayStatics::SetGamePaused(GetWorld(), true);

        // Create and add the pause menu widget
        PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
        if (PauseMenuInstance)
        {
            PauseMenuInstance->AddToViewport(100);
            bShowMouseCursor = true;

            // Set input mode to UI only so that input goes to the widget
            FInputModeUIOnly UIInput;
            UIInput.SetWidgetToFocus(PauseMenuInstance->TakeWidget());
            SetInputMode(UIInput);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create PauseMenuInstance!"));
        }

        // Disable pawn input so the player cannot move while paused
        if (APawn* ControlledPawn = GetPawn())
        {
            ControlledPawn->DisableInput(this);
        }
    }
}

void ASPlayerController::UnpauseGame()
{
    // Only unpause if the game is currently paused.
    if (UGameplayStatics::IsGamePaused(GetWorld()))
    {
        // Unpause the game
        UGameplayStatics::SetGamePaused(GetWorld(), false);

        // Remove the pause menu widget, if present
        if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
        {
            PauseMenuInstance->RemoveFromParent();
            PauseMenuInstance = nullptr;
        }

        // Set input mode back to game only and hide the cursor
        FInputModeGameOnly GameInput;
        SetInputMode(GameInput);
        bShowMouseCursor = false;

        // Re-enable pawn input so the player can control the character
        if (APawn* ControlledPawn = GetPawn())
        {
            ControlledPawn->EnableInput(this);
        }
    }
}

void ASPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Bind your pause/unpause actions.
    // Bind Esc (or an action mapped to Esc) to PauseGame() 
    InputComponent->BindAction("PauseMenu", IE_Pressed, this, &ASPlayerController::PauseGame);
    // Bind Return (or an action mapped to Return) to UnpauseGame()
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
