#include "VictoryScreenWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerState.h"

void UVictoryScreenWidget::NativeConstruct()
{
    Super::NativeConstruct();

    APlayerController* PC = GetOwningPlayer();
    if (PC && PC->PlayerState)
    {
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
