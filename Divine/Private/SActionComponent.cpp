#include "SActionComponent.h"
#include "SAction.h"
#include "Divine.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SActionComponent)

USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	bReplicateUsingRegisteredSubObjectList = true;
}


void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		for (TSubclassOf<USAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}
}


void USActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TArray<USAction*> ActionsCopy = Actions;
	for (USAction* Action : ActionsCopy)
	{
		if (Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}
	}

	Super::EndPlay(EndPlayReason);
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	if (!GetOwner()->HasAuthority())
	{
		UE_LOGFMT(LogGame, Warning, "Client attempting to AddAction. [Class: {Class}]", GetNameSafe(ActionClass));
		return;
	}

	USAction* NewAction = NewObject<USAction>(GetOwner(), ActionClass);
	check(NewAction);

	NewAction->Initialize(this);

	Actions.Add(NewAction);

	AddReplicatedSubObject(NewAction);

	if (NewAction->IsAutoStart() && ensure(NewAction->CanStart(Instigator)))
	{
		NewAction->StartAction(Instigator);
	}
}


void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
	{
		return;
	}

	RemoveReplicatedSubObject(ActionToRemove);

	Actions.Remove(ActionToRemove);
}


USAction* USActionComponent::GetAction(TSubclassOf<USAction> ActionClass) const
{
	for (USAction* Action : Actions)
	{
		if (Action->IsA(ActionClass))
		{
			return Action;
		}
	}

	return nullptr;
}


bool USActionComponent::StartActionByName(AActor* Instigator, FGameplayTag ActionName)
{

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("StartActionByName"), StartActionByName, STATGROUP_STANFORD);

	SCOPED_NAMED_EVENT(StartActionName, FColor::Green);

	for (USAction* Action : Actions)
	{
		if (Action->GetActivationTag() == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
				continue;
			}

			if (!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator, ActionName);
			}

			TRACE_BOOKMARK(TEXT("StartAction::%s"), *GetNameSafe(Action));
			{

				SCOPED_NAMED_EVENT_FSTRING(Action->GetClass()->GetName(), FColor::White);

				Action->StartAction(Instigator);
			}

			return true;
		}
	}

	return false;
}


bool USActionComponent::StopActionByName(AActor* Instigator, FGameplayTag ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action->GetActivationTag() == ActionName)
		{
			if (Action->IsRunning())
			{
				if (!GetOwner()->HasAuthority())
				{
					ServerStopAction(Instigator, ActionName);
				}

				Action->StopAction(Instigator);
				return true;
			}
		}
	}

	return false;
}


void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FGameplayTag ActionName)
{
	StartActionByName(Instigator, ActionName);
}


void USActionComponent::ServerStopAction_Implementation(AActor* Instigator, FGameplayTag ActionName)
{
	StopActionByName(Instigator, ActionName);
}


void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);
}