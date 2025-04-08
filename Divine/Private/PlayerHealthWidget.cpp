#include "PlayerHealthWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "BasePlayer.h"
#include "SAttributeComponent.h"

void UPlayerHealthWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        APawn* Pawn = PC->GetPawn();
        if (Pawn)
        {
            ABasePlayer* Player = Cast<ABasePlayer>(Pawn);
            if (Player && Player->GetAttributeComponent())
            {
                float Health = Player->GetAttributeComponent()->GetHealth();
                float HealthMax = Player->GetAttributeComponent()->GetHealthMax();
                const float Percent = (HealthMax > 0.f) ? (Health / HealthMax) : 0.f;

                if (HealthProgressBar)
                {
                    HealthProgressBar->SetPercent(Percent);
                }

                if (HealthText)
                {
                    HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, HealthMax)));
                }
            }
        }
    }
}
