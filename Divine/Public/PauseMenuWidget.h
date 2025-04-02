#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

// Forward declaration for UTextBlock and USSaveGame.
class UTextBlock;
class USSaveGame;

UCLASS()
class DIVINE_API UPauseMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Called when the Save button is clicked in the UI.
    UFUNCTION(BlueprintCallable, Category = "Save")
    void OnSaveButtonClicked();

protected:
    virtual void NativeConstruct() override;

    // This function handles the subsystem's save completed event.
    UFUNCTION()
    void HandleSaveGameWritten(USSaveGame* SaveObject);

    // Bind this UTextBlock to a TextBlock widget in your UMG design.
    // Make sure the widget is marked as a variable in the UMG designer.
    UPROPERTY(meta = (BindWidget))
    UTextBlock* SaveStatusText;

private:
    FTimerHandle StatusMessageTimerHandle;

    void ShowSaveMessage();
    void ClearStatusMessage();
};
