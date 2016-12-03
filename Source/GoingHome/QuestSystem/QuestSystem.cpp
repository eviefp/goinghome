// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "QuestSystem.h"
#include "Quest.h"
#include "QuestTriggerBase.h"
#include "QuestObjectiveBase.h"
#include "QuestConsequenceBase.h"


AQuestSystem::AQuestSystem(const FObjectInitializer& ObjectInitializer)
{
}

void AQuestSystem::BeginPlay()
{
	auto path = FPaths::Combine(FPaths::GameContentDir(), FString("Data/") + "questsystem.json");

	FString jsonString;
	FFileHelper::LoadFileToString(jsonString, *path);
	TSharedPtr<FJsonObject> jsonObject;
	auto reader = TJsonReaderFactory<>::Create(jsonString);

	if (FJsonSerializer::Deserialize(reader, jsonObject))
	{
		auto quests = jsonObject->GetArrayField("quests");

		for (auto questJson : quests)
		{
			ParseQuest(questJson->AsObject());
		}
	}

	auto startQuest = Quests.Find(FName("default"));
	if (startQuest != nullptr)
	{
		AcquireQuest(*startQuest);
	}
}

void AQuestSystem::AcquireQuest(UQuest* Quest)
{
	ActiveQuests.Add(Quest);

	for (auto trigger : Quest->Triggers)
	{
		trigger->Execute(Quest);
	}
}

void AQuestSystem::OnEvent(EObjectiveEvent objEvent, AActor* Source, AActor* Target, float amount)
{
	for (auto quest : ActiveQuests)
	{
		auto pendingObjectives = 0;

		for (auto objective : quest->Objectives)
		{
			if (!objective->IsCompleted
				&& objective->Event == objEvent
				&& (objective->Source.IsNone() 
					|| (Source != nullptr && objective->Source == Source->GetFName()))
				&& (objective->Target.IsNone() 
					|| (Target != nullptr && objective->Target == Target->GetFName())))
			{
				objective->OnProgress(quest);
			}

			if (!objective->IsCompleted)
			{
				++pendingObjectives;
			}
		}

		if (pendingObjectives == 0)
		{
			quest->IsCompleted = true;

			for (auto complete : quest->CompleteConsequences)
			{
				complete->Execute(quest);
			}
		}
	}
}

void AQuestSystem::ParseQuest(TSharedPtr<FJsonObject> questObject)
{
	UQuest* Quest = nullptr;

	FString inputString;

	/** Parse the ID of the Quest. If we can't find it, we skip this quest. */
	if (questObject->TryGetStringField("id", inputString))
	{
		Quest = NewObject<UQuest>(this);
		Quest->Id = FName(*inputString);
		Quests.Add(Quest->Id, Quest);
	}
	else 
	{
		UE_LOG(GoingHomeQuestSystem, Warning, TEXT("Could not parse quest."));
		return;
	}

	/** Title if we can find it. */
	if (questObject->TryGetStringField("title", inputString))
	{
		Quest->Title = FText::FromString(inputString);
	}

	/** Description, if we can find it. */
	if (questObject->TryGetStringField("description", inputString))
	{
		Quest->Description = FText::FromString(inputString);
	}

	/** Triggers, if we can find any. */
	const TArray<TSharedPtr<FJsonValue>> *triggers;
	if (questObject->TryGetArrayField("triggers", triggers))
	{
		for (auto& trigger : *triggers)
		{
			ParseTrigger(trigger->AsObject(), Quest);
		}
	}

	/** Objectives, if we can find any. */
	const TArray<TSharedPtr<FJsonValue>> *objectives;
	if (questObject->TryGetArrayField("objectives", objectives))
	{
		for (auto& objective : *objectives)
		{
			ParseObjective(objective->AsObject(), Quest);
		}
	}

	/** Completion Consequences, if we can find any. */
	const TArray<TSharedPtr<FJsonValue>> *completed;
	if (questObject->TryGetArrayField("completed", completed))
	{
		for (auto& complete : *completed)
		{
			ParseConsequences(complete->AsObject(), Quest, true);
		}
	}

	/** Failed Consequences, if we can find any. */
	const TArray<TSharedPtr<FJsonValue>> *failed;
	if (questObject->TryGetArrayField("failed", failed))
	{
		for (auto& fail : *failed)
		{
			ParseConsequences(fail->AsObject(), Quest, false);
		}
	}
}

void AQuestSystem::ParseTrigger(TSharedPtr<FJsonObject> triggerObject, UQuest* Quest)
{
	FString idString, classString;

	/** By default we will create a trigger base just so we find the error in the logs. */
	if (!triggerObject->TryGetStringField("type", classString))
	{
		classString = "QuestTriggerBase";
	}

	/** We will only parse the trigger if it has an id. */
	if (!triggerObject->TryGetStringField("id", idString))
	{
		UE_LOG(GoingHomeQuestSystem, Warning, TEXT("Could not parse trigger."));
		return;
	}

	/** Find the class and create / set it up. */
	auto triggerClass = FindObject<UClass>(ANY_PACKAGE, *classString);
	if (triggerClass == nullptr) 
	{
		UE_LOG(GoingHomeQuestSystem, Warning, TEXT("Cannot find class name %s. Default name is %s"),
			*classString,
			*UQuestTriggerBase::StaticClass()->GetName()
		);
		return;
	}

	UQuestTriggerBase* Trigger = NewObject<UQuestTriggerBase>(Quest, triggerClass);

	Trigger->Id = FName(*idString);

	Quest->Triggers.Add(Trigger);
}

void AQuestSystem::ParseObjective(TSharedPtr<FJsonObject> objectiveObject, UQuest* Quest)
{
	FString idString, classString;

	/** By default we will create an objective base just so we find the error in the logs. */
	if (!objectiveObject->TryGetStringField("type", classString))
	{
		classString = "QuestObjectiveBase";
	}

	/** We will only parse the objective if it has an id. */
	if (!objectiveObject->TryGetStringField("id", idString))
	{
		UE_LOG(GoingHomeQuestSystem, Warning, TEXT("Could not parse objective."));
		return;
	}

	/** Find the class and create / set it up. */
	auto objectiveClass = FindObject<UClass>(ANY_PACKAGE, *classString);
	if (objectiveClass == nullptr)
	{
		UE_LOG(GoingHomeQuestSystem, Warning, TEXT("Cannot find class name %s. Default name is %s"),
			*classString,
			*UQuestObjectiveBase::StaticClass()->GetName()
		);
		return;
	}

	UQuestObjectiveBase* Objective = NewObject<UQuestObjectiveBase>(Quest, objectiveClass);

	Objective->Id = FName(*idString);
	Quest->Objectives.Add(Objective);

	/** Parse the non-mandatory fields. */
	FString inputString;

	// Title
	if (objectiveObject->TryGetStringField("title", inputString))
	{
		Objective->Title = FText::FromString(inputString);
	}

	// Description
	if (objectiveObject->TryGetStringField("description", inputString))
	{
		Objective->Description = FText::FromString(inputString);
	}

	// Event enum
	if (objectiveObject->TryGetStringField("event", inputString))
	{
		UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, *FString("EObjectiveEvent"), true);
		if (!Enum)
		{
			Objective->Event = EObjectiveEvent(0);
		}
		Objective->Event = (EObjectiveEvent)Enum->FindEnumIndex(FName(*inputString));
	}

	// Source
	if (objectiveObject->TryGetStringField("source", inputString))
	{
		Objective->Source = FName(*inputString);
	}

	// Target
	if (objectiveObject->TryGetStringField("target", inputString))
	{
		Objective->Target = FName(*inputString);
	}

	int32 inputNumber;
	// Min repetitions
	if (objectiveObject->TryGetNumberField("repetitions", inputNumber))
	{
		Objective->Repetitions = inputNumber;
	}

	bool inputBool;
	// Is Optional
	if (objectiveObject->TryGetBoolField("isOptional", inputBool))
	{
		Objective->IsOptional = inputBool;
	}

	// Is Enabled
	if (objectiveObject->TryGetBoolField("isEnabled", inputBool))
	{
		Objective->IsEnabled = inputBool;
	}

	// Show Objective
	if (objectiveObject->TryGetBoolField("showObjective", inputBool))
	{
		Objective->ShowObjective = inputBool;
	}
}

void AQuestSystem::ParseConsequences(TSharedPtr<FJsonObject> completeObject, class UQuest* Quest, bool complete)
{
	FString idString, classString;

	/** By default we will create an objective base just so we find the error in the logs. */
	if (!completeObject->TryGetStringField("type", classString))
	{
		classString = "QuestConsequenceBase";
	}

	/** We will only parse the objective if it has an id. */
	if (!completeObject->TryGetStringField("id", idString))
	{
		UE_LOG(GoingHomeQuestSystem, Warning, TEXT("Could not parse consequence."));
		return;
	}

	/** Find the class and create / set it up. */
	auto objectiveClass = FindObject<UClass>(ANY_PACKAGE, *classString);
	if (objectiveClass == nullptr)
	{
		UE_LOG(GoingHomeQuestSystem, Warning, TEXT("Cannot find class name %s. Default name is %s"),
			*classString,
			*UQuestConsequenceBase::StaticClass()->GetName()
		);
		return;
	}

	auto* Consequence = NewObject<UQuestConsequenceBase>(Quest, objectiveClass);

	Consequence->Id = FName(*idString);
	if (complete)
	{
		Quest->CompleteConsequences.Add(Consequence);
	}
	else
	{
		Quest->FailConsequences.Add(Consequence);
	}

	/** Parse the non-mandatory fields. */
	FString inputString;

	// Title
	if (completeObject->TryGetStringField("title", inputString))
	{
		Consequence->Title = FText::FromString(inputString);
	}

	// Description
	if (completeObject->TryGetStringField("description", inputString))
	{
		Consequence->Description = FText::FromString(inputString);
	}
}