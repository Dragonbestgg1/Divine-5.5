#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UTextBlock;
class USSaveGame;

UCLASS()
class DIVINE_API UPauseMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Save")
    void OnSaveButtonClicked();

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void HandleSaveGameWritten(USSaveGame* SaveObject);

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SaveStatusText;

private:
    FTimerHandle StatusMessageTimerHandle;

    void ShowSaveMessage();
    void ClearStatusMessage();
};
