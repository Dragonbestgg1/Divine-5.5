// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SAction.generated.h"

class UWorld;
class USActionComponent;

USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool bIsRunning;

	UPROPERTY()
	TObjectPtr<AActor> Instigator;

	FActionRepData()
	{
		bIsRunning = false;
	}
};

UCLASS(Blueprintable)
class DIVINE_API USAction : public UObject
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(Replicated)
	TObjectPtr<USActionComponent> ActionComp;

	UFUNCTION(BlueprintCallable, Category = "Action")
	USActionComponent* GetOwningComponent() const;

	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantsTags;

	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	UPROPERTY(ReplicatedUsing = "OnRep_RepData")
	FActionRepData RepData;

	UPROPERTY(Replicated)
	float TimeStarted;

	UFUNCTION()
	void OnRep_RepData();

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FGameplayTag ActivationTag;

public:

	UFUNCTION(BlueprintPure)
	FGameplayTag GetActivationTag() const
	{
		return ActivationTag;
	}

	UFUNCTION(BlueprintPure)
	bool IsAutoStart() const
	{
		return bAutoStart;
	}

	void Initialize(USActionComponent* NewActionComp);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool IsRunning() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	virtual UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}
};
