// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "GoingHomeGameState.h"

AGoingHomeGameState::AGoingHomeGameState()
{
	CurrentEventIndex = -1;
	timerSet = false;
}

void AGoingHomeGameState::BeginPlay()
{
	auto levelName = GetWorld()->GetName();
	auto path = FPaths::Combine(FPaths::GameContentDir(), FString("Data/") + levelName + ".json");

	FString jsonString;

	FFileHelper::LoadFileToString(jsonString, *path);

	TSharedPtr<FJsonObject> jsonObject;

	auto reader = TJsonReaderFactory<>::Create(jsonString);

	if(FJsonSerializer::Deserialize(reader, jsonObject))
	{
		auto events = jsonObject->GetArrayField("events");
		for(auto& ev : events)
		{
			auto obj = ev->AsObject();
			Events.AddDefaulted(1);
			auto& storedEvent = Events.Top();

			FString inputString;
			int32 inputNumber;

			if(obj->TryGetStringField("eventId", inputString))
			{
				storedEvent.EventId = FName(*inputString);
			}
			else
			{
				UE_LOG(GoingHomeLog, Warning, TEXT("skipping bad event for index %d"), Events.Num());
				Events.Pop();
				continue;
			}

			if(obj->TryGetNumberField("time", inputNumber))
			{
				storedEvent.Time = inputNumber;
			}
			
			if (obj->TryGetStringField("text", inputString))
			{
				storedEvent.Text = FText::FromString(inputString);
			}
			else
			{
				UE_LOG(GoingHomeLog, Warning, TEXT("Empty text for event %s"), *storedEvent.EventId.ToString());
				storedEvent.Text = FText::GetEmpty();
			}


			if(obj->TryGetStringField("afterEvent", inputString))
			{
				storedEvent.AfterEvent = FName(*inputString);
			}
			else
			{
				storedEvent.AfterEvent = "";
			}


			TArray< FString > playerActions;
			if(obj->TryGetStringArrayField("playerActions", playerActions))
			{
				for(auto& action: playerActions)
				{
					storedEvent.PlayerActions.Add(FName(*action));
				}
			}

			TArray< FString > playerActionArguments;
			if (obj->TryGetStringArrayField("playerActionArguments", playerActionArguments))
			{
				for (auto& action : playerActionArguments)
				{
					storedEvent.PlayerActionArguments.Add(FName(*action));
				}
			}
		}
		// Assume first event can't have AfterEvent, so we just schedule it.
		CurrentEventIndex = -1;
		TimerHandler();
	}
	else
	{
		UE_LOG(GoingHomeLog, Error, TEXT("Could not deserialize path %s"), *path);
	}
}

void AGoingHomeGameState::TimerHandler()
{
	++CurrentEventIndex;
	timerSet = false;

	UE_LOG(GoingHomeLog, Log, TEXT("CurrentIndex %d"), CurrentEventIndex);

	if(Events.Num() == CurrentEventIndex)
	{
		--CurrentEventIndex;
		return;
	}

	auto& currentEvent = Events[CurrentEventIndex];
	DisplayText = currentEvent.Text.ToString();

	if(currentEvent.PlayerActions.Num() == 0 && currentEvent.Time > 0)
	{
		UE_LOG(GoingHomeLog, Log, TEXT("Setting timer"));
		GetWorldTimerManager().SetTimer(EventTimerHandle, this, &AGoingHomeGameState::TimerHandler, currentEvent.Time);
	}
	else
	{
		UE_LOG(GoingHomeLog, Log, TEXT("Skipping set timer"));
	}
}

void AGoingHomeGameState::OnPitch(float value)
{
	HandleShipEvent("pitch");
}

void AGoingHomeGameState::OnYaw(float value)
{
	HandleShipEvent("yaw");
}

void AGoingHomeGameState::OnRoll(float value)
{
	HandleShipEvent("roll");
}

void AGoingHomeGameState::OnThrust(float value)
{
	if (value > 0)
		HandleShipEvent("thrust");
	else if (value < 0)
		HandleShipEvent("thrustBackwards");
}

void AGoingHomeGameState::OnOverlap(FName other)
{
	HandleInteractionEvent("overlap", other);
}

void AGoingHomeGameState::OnMine(FName other)
{
	HandleInteractionEvent("mine", other);
}

void AGoingHomeGameState::OnMineDestroyed(FName other)
{
	HandleInteractionEvent("mineDestroyed", other);
}

void AGoingHomeGameState::OnProjectileHit(AActor const* shooter, AActor const* victim)
{
	if(shooter->GetName() == "ShippyMcShipFace")
		HandleInteractionEvent("projectileHit", victim->GetFName());
}

void AGoingHomeGameState::HandleInteractionEvent(FName type, FName other)
{
	if (CurrentEventIndex < 0)
		return;

	auto& currentEvent = Events[CurrentEventIndex];

	for (auto i = 0; i < currentEvent.PlayerActions.Num(); ++i)
	{
		auto& action = currentEvent.PlayerActions[i];

		if (currentEvent.PlayerActionArguments.Num() <= i) return;

		if (action == type && currentEvent.PlayerActionArguments[i] == other)
		{
			HandleShipEvent(i);
			return;
		}
	}
}

void AGoingHomeGameState::HandleShipEvent(FName type)
{
	if (CurrentEventIndex < 0)
		return;

	auto index = Events[CurrentEventIndex].PlayerActions.Find(type);

	HandleShipEvent(index);
}

void AGoingHomeGameState::HandleShipEvent(int index)
{
	auto& currentEvent = Events[CurrentEventIndex];

	if (index >= 0)
	{
		currentEvent.PlayerActions.RemoveAt(index);
	}

	if (currentEvent.PlayerActions.Num() == 0 && !timerSet) 
	{
		timerSet = true;
		GetWorldTimerManager().SetTimer(EventTimerHandle, this, &AGoingHomeGameState::TimerHandler, currentEvent.Time);
	}
}
