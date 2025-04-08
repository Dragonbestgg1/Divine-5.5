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
	UPROPERTY(meta = (BindWidget))
	USizeBox* ParentSizeBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
