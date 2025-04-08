#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "SSaveGameSettings.generated.h"

class UDataTable;
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Save Game Settings"))
class DIVINE_API USSaveGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FString SaveSlotName;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General", AdvancedDisplay)
	TSoftObjectPtr<UDataTable> DummyTablePath;

	USSaveGameSettings();
};
