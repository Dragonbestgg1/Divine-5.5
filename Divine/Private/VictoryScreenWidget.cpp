#include "VictoryScreenWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerState.h" // Make sure this header matches the one in your project

void UVictoryScreenWidget::NativeConstruct()
{
    Super::NativeConstruct();

    APlayerController* PC = GetOwningPlayer();
    if (PC && PC->PlayerState)
    {
        // Assuming your player state class is ASPlayerState with a GetCredits() method
        if (ASPlayerState* PS = Cast<ASPlayerState>(PC->PlayerState))
        {
            int32 FinalScore = PS->GetCredits();

            if (ScoreText)
            {
                ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), FinalScore)));
            }
        }
    }
}
