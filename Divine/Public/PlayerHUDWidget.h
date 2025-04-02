#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS(Blueprintable)
class DIVINE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** Progress bar showing the player's health. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	/** Text block showing the player's numeric health (e.g., "95 / 100"). */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	/** Text block showing the player's score. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	/** Update the widget each frame. */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
