// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "QuestObjectiveBase.h"
#include "Quest.h"


void UQuestObjectiveBase::Parse(TSharedPtr<FJsonObject> objectiveObject)
{
	/** Parse the non-mandatory fields. */
	FString inputString;

	// Title
	if (objectiveObject->TryGetStringField("title", inputString))
	{
		Title = FText::FromString(inputString);
	}

	// Description
	if (objectiveObject->TryGetStringField("description", inputString))
	{
		Description = FText::FromString(inputString);
	}

	// Event enum
	if (objectiveObject->TryGetStringField("event", inputString))
	{
		UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, *FString("EObjectiveEvent"), true);
		if (!Enum)
		{
			Event = EObjectiveEvent(0);
		}
		Event = (EObjectiveEvent)Enum->FindEnumIndex(FName(*inputString));
	}

	// Source
	if (objectiveObject->TryGetStringField("source", inputString))
	{
		Source = FName(*inputString);
	}

	// Target
	if (objectiveObject->TryGetStringField("target", inputString))
	{
		Target = FName(*inputString);
	}

	int32 inputNumber;
	// Min repetitions
	if (objectiveObject->TryGetNumberField("repetitions", inputNumber))
	{
		Repetitions = inputNumber;
	}

	bool inputBool;
	// Is Optional
	if (objectiveObject->TryGetBoolField("isOptional", inputBool))
	{
		IsOptional = inputBool;
	}
	else
	{
		IsOptional = false;
	}

	// Is Enabled
	if (objectiveObject->TryGetBoolField("isEnabled", inputBool))
	{
		IsEnabled = inputBool;
	}
	else
	{
		IsEnabled = true;
	}

	// Show Objective
	if (objectiveObject->TryGetBoolField("showObjective", inputBool))
	{
		ShowObjective = inputBool;
	}
	else
	{
		ShowObjective = true;
	}
}

void UQuestObjectiveBase::OnProgress_Implementation(UQuest* Quest)
{
	UQuest::LogQuestTrace(Quest, FString("UQuestObjectiveBase Progress"));
	++CurrentRepetitions;
	IsCompleted = CurrentRepetitions == Repetitions;

	if (IsCompleted)
		OnComplete(Quest);
}

void UQuestObjectiveBase::OnComplete_Implementation(UQuest* Quest)
{
	UQuest::LogQuestTrace(Quest, FString("UQuestObjectiveBase Completed"));
}
