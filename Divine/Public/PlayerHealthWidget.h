#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHealthWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS(Blueprintable)
class DIVINE_API UPlayerHealthWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Bind this in the UMG designer to a ProgressBar.
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	// (Optional) Bind this to a TextBlock that shows health numbers.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
