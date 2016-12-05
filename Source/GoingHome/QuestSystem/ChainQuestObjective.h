// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "QuestSystem/QuestObjectiveBase.h"
#include "ChainQuestObjective.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class GOINGHOME_API UChainQuestObjective : public UQuestObjectiveBase
{
	GENERATED_BODY()
	
	
public:
	//~=============================================================================
	// Objective Properties

	/** Parse the fields, except the Id. That is mandatory and set by the instigator. */
	virtual void Parse(TSharedPtr<FJsonObject> objectiveObject);
	

	//~=============================================================================
	// Objective Properties

	/** Internal ID that can be referenced by other quest triggers / objectives / etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NextObjectiveId;


	//~=============================================================================
	// Objective Events

	virtual void OnComplete_Implementation(UQuest* Quest);
};
