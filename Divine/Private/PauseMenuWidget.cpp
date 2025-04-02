#include "PauseMenuWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "SSaveGameSubsystem.h"
#include "Engine/GameInstance.h"

void UPauseMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Clear any previous message when the widget is constructed.
    if (SaveStatusText)
    {
        SaveStatusText->SetText(FText::GetEmpty());
    }

    // Bind to the subsystem's OnSaveGameWritten delegate.
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (USSaveGameSubsystem* SaveSubsystem = GameInstance->GetSubsystem<USSaveGameSubsystem>())
        {
            SaveSubsystem->OnSaveGameWritten.AddDynamic(this, &UPauseMenuWidget::HandleSaveGameWritten);
        }
    }
}

void UPauseMenuWidget::OnSaveButtonClicked()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (USSaveGameSubsystem* SaveSubsystem = GameInstance->GetSubsystem<USSaveGameSubsystem>())
        {
            SaveSubsystem->WriteSaveGame();
            UE_LOG(LogTemp, Log, TEXT("Game saved via pause menu."));
            // No need to call ShowSaveMessage() here; it will be triggered via the delegate.
        }
    }
}

void UPauseMenuWidget::HandleSaveGameWritten(USSaveGame* /*SaveObject*/)
{
    // Once the subsystem has finished saving, display the message.
    ShowSaveMessage();
}

void UPauseMenuWidget::ShowSaveMessage()
{
    if (SaveStatusText)
    {
        SaveStatusText->SetText(FText::FromString("Game Saved!"));
        // Clear the message after 2 seconds.
        GetWorld()->GetTimerManager().SetTimer(StatusMessageTimerHandle, this, &UPauseMenuWidget::ClearStatusMessage, 2.0f, false);
    }
}

void UPauseMenuWidget::ClearStatusMessage()
{
    if (SaveStatusText)
    {
        SaveStatusText->SetText(FText::GetEmpty());
    }
}
