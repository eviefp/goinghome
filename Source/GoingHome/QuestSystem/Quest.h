// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "Quest.generated.h"

/**
*
*/
UCLASS()
class GOINGHOME_API UQuest : public UObject
{
	GENERATED_BODY()


public:
	//~=============================================================================
	// Quest Properties

	/** Internal ID that can be referenced by other quest triggers / objectives / etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName Id;

	/** The quest name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText Title;

	/** Detailed information about the quest. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText Description;

	/** Executable code you can plug when this quest starts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TArray<class UQuestTriggerBase*> Triggers;

	/** Objectives related to this quest. Some may be optional. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TArray<class UQuestObjectiveBase*> Objectives;

	/**
	* What happens when all objectives are completed. Some might only happen if bonus
	* objectives are met.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TArray<class UQuestConsequenceBase*> CompleteConsequences;

	/** What happens when one mandatory objective is failed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TArray<class UQuestConsequenceBase*> FailConsequences;


	//~=============================================================================
	// Helper methods

	UFUNCTION(BlueprintCallable, Category = "QuestHelpers")
	static void LogQuestTrace(const UQuest* Quest, FString caller);

};
