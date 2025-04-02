#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerScoreWidget.generated.h"

class USizeBox;
class UTextBlock;

UCLASS(Blueprintable)
class DIVINE_API UPlayerScoreWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// A container for positioning the widget.
	UPROPERTY(meta = (BindWidget))
	USizeBox* ParentSizeBox;

	// Text block that displays the player's score.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
