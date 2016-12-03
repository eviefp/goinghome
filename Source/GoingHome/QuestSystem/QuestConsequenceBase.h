// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "QuestConsequenceBase.generated.h"

/**
 * 
 */
UCLASS(abstract)
class GOINGHOME_API UQuestConsequenceBase : public UObject
{
	GENERATED_BODY()
	
	
public:
	//~=============================================================================
	// Execute trigger

	UFUNCTION(BlueprintNativeEvent, Category = "Quest")
	void Execute(class UQuest* Quest);

	virtual void Execute_Implementation(const UQuest* Quest);
	
};
