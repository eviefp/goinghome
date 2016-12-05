// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "QuestObjectiveBase.generated.h"

UENUM(BlueprintType)
enum class EObjectiveEvent : uint8
{
	OT_Pitch			UMETA(DisplayName = "Pitch"),
	OT_Roll				UMETA(DisplayName = "Roll"),
	OT_Yaw				UMETA(DisplayName = "Yaw"),
	OT_Thrust			UMETA(DisplayName = "Thrust"),
	OT_Trigger			UMETA(DisplayName = "Trigger"),
	OT_Mine				UMETA(DisplayName = "Mine"),
	OT_MineDestroyed	UMETA(DisplayName = "Mine Destroyed"),
	OT_Damaged			UMETA(DisplayName = "Damaged"),
	OT_Destroyed		UMETA(DisplayName = "Destroyed")
};


/**
 * 
 */
UCLASS(BlueprintType)
class GOINGHOME_API UQuestObjectiveBase : public UObject
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
	FName Id;

	/** The quest name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	/** Detailed information about the quest. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/** Objective type. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EObjectiveEvent Event;

	/** Source, if applicable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Source;

	/** Target, if applicable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Target;

	/** How many times to complete objective. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Repetitions;

	/** How many times we repeated this. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentRepetitions;

	/** Is this objective optional? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsOptional;

	/** Enable by default? Can be used to chain objectives. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsEnabled;

	/** Show by default? Can be used for hidden objectives. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowObjective;

	/** Current state of the objective. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCompleted;


	//~=============================================================================
	// Objective Events

	UFUNCTION(BlueprintNativeEvent, Category = "Quest")
	void OnProgress(UQuest* Quest);

	virtual void OnProgress_Implementation(UQuest* Quest);


	UFUNCTION(BlueprintNativeEvent, Category = "Quest")
	void OnComplete(UQuest* Quest);

	virtual void OnComplete_Implementation(UQuest* Quest);
};
