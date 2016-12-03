// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "QuestTriggerBase.generated.h"


/**
 * Base class for quest triggers.
 */
UCLASS()
class GOINGHOME_API UQuestTriggerBase : public UObject
{
	GENERATED_BODY()
	

public:
	//~=============================================================================
	// Execute trigger

	UFUNCTION(BlueprintNativeEvent, Category = "Quest")
	void Execute(class UQuest* Quest);

	virtual void Execute_Implementation(const UQuest* Quest);
	
};
