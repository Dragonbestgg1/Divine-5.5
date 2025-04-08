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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> ParentSizeBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

};
