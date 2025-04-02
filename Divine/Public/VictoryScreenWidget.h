#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VictoryScreenWidget.generated.h"

class UTextBlock;

UCLASS()
class DIVINE_API UVictoryScreenWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    // Bind this to your Score Text Block in the widget designer
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ScoreText;
};
