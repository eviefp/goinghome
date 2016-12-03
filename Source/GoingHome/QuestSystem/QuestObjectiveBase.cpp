// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "QuestObjectiveBase.h"
#include "Quest.h"


void UQuestObjectiveBase::OnProgress_Implementation(const UQuest* Quest)
{
	UQuest::LogQuestTrace(Quest, FString("UQuestObjectiveBase Execute"));
}

void UQuestObjectiveBase::OnComplete_Implementation(const UQuest* Quest)
{
	UQuest::LogQuestTrace(Quest, FString("UQuestObjectiveBase Execute"));
}
