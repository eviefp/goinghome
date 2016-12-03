// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "QuestObjectiveBase.h"
#include "Quest.h"


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
