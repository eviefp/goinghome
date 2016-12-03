// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "Quest.h"


void UQuest::LogQuestTrace(const UQuest* Quest, FString caller)
{
	if (Quest != nullptr)
	{
		UE_LOG(GoingHomeQuestSystem, Log, TEXT("%s called for Quest %s"), *caller, *Quest->Id.ToString());
	}
	else
	{
		UE_LOG(GoingHomeQuestSystem, Log, TEXT("%s called for null Quest"), *caller);
	}
}


