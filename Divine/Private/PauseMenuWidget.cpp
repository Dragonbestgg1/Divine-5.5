#include "PauseMenuWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "SSaveGameSubsystem.h"
#include "Engine/GameInstance.h"

void UPauseMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SaveStatusText)
    {
        SaveStatusText->SetText(FText::GetEmpty());
    }

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
        }
    }
}

void UPauseMenuWidget::HandleSaveGameWritten(USSaveGame* /*SaveObject*/)
{
    ShowSaveMessage();
}

void UPauseMenuWidget::ShowSaveMessage()
{
    if (SaveStatusText)
    {
        SaveStatusText->SetText(FText::FromString("Game Saved!"));
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
