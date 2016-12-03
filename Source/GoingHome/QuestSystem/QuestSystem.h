// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Info.h"
#include "QuestObjectiveBase.h"
#include "QuestSystem.generated.h"

/**
 * 
 */
UCLASS()
class GOINGHOME_API AQuestSystem : public AInfo
{
	GENERATED_BODY()
	
	
public:
	//~=============================================================================
	// Constructors and overrides.

	/**  */
	AQuestSystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Initialisation that depends on other actors being alive or available. */
	virtual void BeginPlay() override;


	//~=============================================================================
	// Constructors and overrides.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Quest")
	TMap<FName, class UQuest*> Quests;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Quest")
	TArray<class UQuest*> ActiveQuests;

	/** Virtual function? Event? Not sure. */
	void AcquireQuest(class UQuest* Quest);

	/** Virtual function? Event? Not sure. */
	void OnEvent(EObjectiveEvent objEvent, AActor* Source, AActor* Target, float amount);

	
private:
	//~=============================================================================
	// JSON Parse helpers

	void ParseQuest(TSharedPtr<FJsonObject> quest);
	void ParseTrigger(TSharedPtr<FJsonObject> triggerObject, class UQuest* Quest);
	void ParseObjective(TSharedPtr<FJsonObject> objectiverObject, class UQuest* Quest);
	void ParseConsequences(TSharedPtr<FJsonObject> completeObject, class UQuest* Quest, bool complete = true);
};
