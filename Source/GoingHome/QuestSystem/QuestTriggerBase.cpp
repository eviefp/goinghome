// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "Quest.h"
#include "QuestTriggerBase.h"


void UQuestTriggerBase::Execute_Implementation(const UQuest* Quest)
{
	UQuest::LogQuestTrace(Quest, FString("UQuestTriggerBase Execute"));
}

