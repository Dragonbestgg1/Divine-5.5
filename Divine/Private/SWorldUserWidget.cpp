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

	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);

	const float Margin = 10.0f;

	const float ScoreTextWidth = (ScoreText ? ScoreText->GetDesiredSize().X : 100.0f);

	FVector2D DesiredPosition = FVector2D(ViewportSize.X - ScoreTextWidth - Margin, Margin);

	if (ParentSizeBox)
	{
		ParentSizeBox->SetRenderTranslation(DesiredPosition);
	}

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
