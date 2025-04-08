#include "PlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "BasePlayer.h"
#include "SAttributeComponent.h"
#include "SPlayerState.h"

void UPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	if (APawn* Pawn = PC->GetPawn())
	{
		if (ABasePlayer* Player = Cast<ABasePlayer>(Pawn))
		{
			if (USAttributeComponent* AttrComp = Player->GetAttributeComponent())
			{
				float Health = AttrComp->GetHealth();
				float HealthMax = AttrComp->GetHealthMax();
				float HealthPercent = (HealthMax > 0.f) ? (Health / HealthMax) : 0.f;

				if (HealthProgressBar)
				{
					HealthProgressBar->SetPercent(HealthPercent);
				}

				if (HealthText)
				{
					HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, HealthMax)));
				}
			}
		}
	}

	if (PC->PlayerState)
	{
		if (ASPlayerState* PS = Cast<ASPlayerState>(PC->PlayerState))
		{
			if (ScoreText)
			{
				int32 CurrentScore = PS->GetCredits();
				ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), CurrentScore)));
			}
		}
	}
}
