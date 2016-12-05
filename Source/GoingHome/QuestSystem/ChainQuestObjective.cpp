// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "ChainQuestObjective.h"
#include "Quest.h"


void UChainQuestObjective::Parse(TSharedPtr<FJsonObject> objectiveObject)
{
	Super::Parse(objectiveObject);

	FString inputString;

	// Next quest
	if (objectiveObject->TryGetStringField("nextObjectiveId", inputString))
	{
		NextObjectiveId = FName(*inputString);
	}

	bool inputBool;

	if (!objectiveObject->TryGetBoolField("isEnabled", inputBool))
	{
		IsEnabled = false;
	}
}

void UChainQuestObjective::OnComplete_Implementation(UQuest* Quest)
{
	if (!NextObjectiveId.IsNone())
	{
		for (auto objective : Quest->Objectives)
		{
			if (objective->Id == NextObjectiveId)
			{
				objective->IsEnabled = true;
			}
		}
	}

	UQuest::LogQuestTrace(Quest, FString("UChainQuestObjective Completed"));
}
