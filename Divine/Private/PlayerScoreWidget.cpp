#include "PlayerScoreWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "SPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerScoreWidget)

void UPlayerScoreWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
	const float Margin = 10.f;
	const float ScoreWidth = (ScoreText ? ScoreText->GetDesiredSize().X : 100.f);
	const FVector2D DesiredPosition = FVector2D(ViewportSize.X - ScoreWidth - Margin, Margin);
	if (ParentSizeBox)
	{
		ParentSizeBox->SetRenderTranslation(DesiredPosition);
	}

	APlayerController* PC = GetOwningPlayer();
	if (PC && PC->PlayerState)
	{
		if (ASPlayerState* PS = Cast<ASPlayerState>(PC->PlayerState))
		{
			if (ScoreText)
			{
				ScoreText->SetText(FText::AsNumber(PS->GetCredits()));
			}
		}
	}
}
