// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "GoingHomeGameState.h"
#include "EnemyPawn.h"
#include "ProjectileDamageType.h"

AGoingHomeGameState::AGoingHomeGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Initialise();
}

void AGoingHomeGameState::BeginPlay()
{
	// Get the current level name. There should be a similarly named json file in the Data folder for events.
	auto levelName = GetWorld()->GetName();
	auto path = FPaths::Combine(FPaths::GameContentDir(), FString("Data/") + levelName + ".json");

	FString jsonString;
	FFileHelper::LoadFileToString(jsonString, *path);
	TSharedPtr<FJsonObject> jsonObject;
	auto reader = TJsonReaderFactory<>::Create(jsonString);

	if (FJsonSerializer::Deserialize(reader, jsonObject))
	{
		auto events = jsonObject->GetArrayField("events");
		for (auto& ev : events)
		{
			auto obj = ev->AsObject();

			// There is probably a better way, but I'm scared to create a local variable since I think it will get copy constructed/deleted etc.
			Events.AddDefaulted(1);
			auto& storedEvent = Events.Top();

			FString inputString;
			int32 inputNumber;

			// If the event has no eventId, skip it.
			if (obj->TryGetStringField("eventId", inputString))
			{
				storedEvent.EventId = FName(*inputString);
			}
			else
			{
				// This is why it's awkward to use AddDefaulted(1), but it's probably not that important unless we have lots of malformed events.
				UE_LOG(GoingHomeGameState, Warning, TEXT("skipping bad event for index %d"), Events.Num());
				Events.Pop();
				continue;
			}

			// Time can be 0, which will be the default constructed value anyway.
			if (obj->TryGetNumberField("time", inputNumber))
			{
				storedEvent.Time = inputNumber;
			}

			// Text should generally not be empty, but it's not critical so we only log a warning.
			if (obj->TryGetStringField("text", inputString))
			{
				storedEvent.Text = FText::FromString(inputString);
			}
			else
			{
				UE_LOG(GoingHomeGameState, Warning, TEXT("Empty text for event %s"), *storedEvent.EventId.ToString());
				storedEvent.Text = FText::GetEmpty();
			}

			// Currently not used.
			if (obj->TryGetStringField("afterEvent", inputString))
			{
				storedEvent.AfterEvent = FName(*inputString);
			}
			else
			{
				storedEvent.AfterEvent = "";
			}

			// TODO: this should be refactored to be an TArray of object, not string.
			TArray<FString> playerActions;
			if (obj->TryGetStringArrayField("playerActions", playerActions))
			{
				for (auto& action: playerActions)
				{
					storedEvent.PlayerActions.Add(FName(*action));
				}
			}

			// TODO: this should get removed when the above is refactored properly. The by index thing is awkward.
			TArray<FString> playerActionArguments;
			if (obj->TryGetStringArrayField("playerActionArguments", playerActionArguments))
			{
				for (auto& action : playerActionArguments)
				{
					storedEvent.PlayerActionArguments.Add(FName(*action));
				}
			}
		}
		// Assume first event can't have AfterEvent, so we just schedule it. Not sure if we need to set CurrentEventIndex here again after the ctor.
		// Maybe if this object doesn't get destroyed / reconstructed, then it's useful.
		Initialise();
		TimerHandler();
	}
	else
	{
		// Missing or malformed json file.
		UE_LOG(GoingHomeGameState, Error, TEXT("Could not deserialize path %s"), *path);
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
	++MinedAmount;
	HandleInteractionEvent("mine", other);
}

void AGoingHomeGameState::OnMineDestroyed(FName other)
{
	HandleInteractionEvent("mineDestroyed", other);
}

void AGoingHomeGameState::OnProjectileHit(AActor* shooter, AActor* victim)
{
	// This should probably be done differnetly.
	if (shooter->GetName() == "ShippyMcShipFace")
		HandleInteractionEvent("projectileHit", victim->GetFName());

	auto enemyPawn = Cast<AEnemyPawn>(victim);
	auto playerPawn = Cast<APawn>(shooter);
	if (enemyPawn != nullptr && shooter != nullptr)
	{
		auto damageEvent = FDamageEvent(UProjectileDamageType::StaticClass());

		enemyPawn->TakeDamage(50, damageEvent, playerPawn->GetController(), shooter);

		if (enemyPawn->CurrentHitPoints <= 0)
		{
			enemyPawn->Destroy();
		}
	}
}

void AGoingHomeGameState::Initialise()
{
	// This will likely be removed if we start replicating? this value.
	MinedAmount = 0;
	// We always start by incrementing the current event index, so the first has to be -1.
	CurrentEventIndex = -1;
	// Timer will be set by BeginPlay.
	timerSet = false;
}

void AGoingHomeGameState::TimerHandler()
{
	// Time for next event. Timer is not yet set.
	++CurrentEventIndex;
	timerSet = false;

	// If we've reached the end of our array, stop.
	if (Events.Num() == CurrentEventIndex)
	{
		UE_LOG(GoingHomeGameState, Log, TEXT("Completed dialog events."));
		--CurrentEventIndex;
		return;
	}

	auto& currentEvent = Events[CurrentEventIndex];

	// Set the text for the HUD blueprint to read.
	DisplayText = currentEvent.Text.ToString();

	// If we have no PlayerActions to track and we have a time, then setup the next timer.
	if (currentEvent.PlayerActions.Num() == 0 && currentEvent.Time > 0)
	{
		UE_LOG(GoingHomeGameState, Log, TEXT("Setting timer of %s (%d) to %d"), *currentEvent.EventId.ToString(), CurrentEventIndex, currentEvent.Time);
		GetWorldTimerManager().SetTimer(EventTimerHandle, this, &AGoingHomeGameState::TimerHandler, currentEvent.Time);
	}
	else
	{
		UE_LOG(GoingHomeGameState, Log, TEXT("Skipping timer of %s (%d)"), *currentEvent.EventId.ToString(), CurrentEventIndex);
	}
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
		if (currentEvent.PlayerActionArguments.Num() > index)
			currentEvent.PlayerActionArguments.RemoveAt(index);
	}

	if (currentEvent.PlayerActions.Num() == 0 && !timerSet)
	{
		timerSet = true;
		GetWorldTimerManager().SetTimer(EventTimerHandle, this, &AGoingHomeGameState::TimerHandler, currentEvent.Time);
	}
}
