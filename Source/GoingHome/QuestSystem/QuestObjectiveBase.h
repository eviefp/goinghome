// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "QuestObjectiveBase.generated.h"

UENUM(BlueprintType)
enum class EObjectiveType : uint8
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
UCLASS()
class GOINGHOME_API UQuestObjectiveBase : public UObject
{
	GENERATED_BODY()
	
	
public:
	//~=============================================================================
	// Objective Properties

	/** Internal ID that can be referenced by other quest triggers / objectives / etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName Id;

	/** The quest name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText Title;

	/** Detailed information about the quest. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText Description;

	/** Objective type. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	EObjectiveType Type;

	/** Source, if applicable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName Source;

	/** Target, if applicable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName Target;

	/** How many times to complete objective. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 MinRepetitions;

	/** How many times extra are counted to complete objective. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 MaxRepetitions;

	/** Is this objective optional? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool IsOptional;

	/** Enable by default? Can be used to chain objectives. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool IsEnabled;

	/** Show by default? Can be used for hidden objectives. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool ShowObjective;


	//~=============================================================================
	// Objective Events

	UFUNCTION(BlueprintNativeEvent, Category = "Quest")
	void OnProgress(class UQuest* Quest);

	virtual void OnProgress_Implementation(const UQuest* Quest);


	UFUNCTION(BlueprintNativeEvent, Category = "Quest")
	void OnComplete(class UQuest* Quest);

	virtual void OnComplete_Implementation(const UQuest* Quest);
};
