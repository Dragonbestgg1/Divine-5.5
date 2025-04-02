#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"

class USizeBox;
class UTextBlock;

UCLASS()
class DIVINE_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// The container widget used to move the widget to the desired location.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> ParentSizeBox;

	// Text block that displays the player's score.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	// Override the tick so we can reposition and update the score.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	// You can add additional customization properties here if needed.
};
