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
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
