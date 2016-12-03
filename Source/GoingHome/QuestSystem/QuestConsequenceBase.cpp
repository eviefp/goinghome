// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "Quest.h"
#include "QuestConsequenceBase.h"


void UQuestConsequenceBase::Execute_Implementation(const UQuest* Quest)
{
	UQuest::LogQuestTrace(Quest, FString("UQuestConsequenceBase Execute"));
}

