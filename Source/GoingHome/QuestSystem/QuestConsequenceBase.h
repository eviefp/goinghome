// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "QuestConsequenceBase.generated.h"

/**
 * 
 */
UCLASS()
class GOINGHOME_API UQuestConsequenceBase : public UObject
{
	GENERATED_BODY()
	
	
public:
	/** Internal ID that can be referenced by other quest triggers / objectives / etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Id;

	/** The quest name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	/** Detailed information about the quest. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;


	//~=============================================================================
	// Execute trigger

	UFUNCTION(BlueprintNativeEvent, Category = "Quest")
	void Execute(class UQuest* Quest);

	virtual void Execute_Implementation(const UQuest* Quest);
	
};
