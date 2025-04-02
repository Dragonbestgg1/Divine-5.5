#include "SWorldUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "SPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SWorldUserWidget)

void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Get the current viewport size.
	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);

	// Margin from the edges.
	const float Margin = 10.0f;

	// Get the width of the ScoreText, defaulting to 100 if not valid.
	const float ScoreTextWidth = (ScoreText ? ScoreText->GetDesiredSize().X : 100.0f);

	// Calculate the position for the score widget: top-right corner.
	FVector2D DesiredPosition = FVector2D(ViewportSize.X - ScoreTextWidth - Margin, Margin);

	// Set the ParentSizeBox position so that the widget is always at the desired location.
	if (ParentSizeBox)
	{
		ParentSizeBox->SetRenderTranslation(DesiredPosition);
	}

	// Get the owning player's state and update the ScoreText with the current score.
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (PC->PlayerState)
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
}
