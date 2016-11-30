// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * EnemyAIController sends MoveTo commands to the EnemyPawn. It receives OnTargetReached
 * from the EnemyPawn when target is reached and sends the next one.
 * It also receives OnTargetInSight events from EnemyPawn's Sight component.
 */
UCLASS()
class GOINGHOME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	

public:
	//~=============================================================================
	// Constructors and overrides.
	AEnemyAIController(const FObjectInitializer& ObjectInitializer);

	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;


	//~=============================================================================
	// EnemyPawn / SightComponent Interface
	
	/** Called by EnemyPawn when it has reached its target (within Epsilon). */
	virtual void OnTargetReached();

	/** Called by EnemySightComponent when the player is in sight. */
	virtual void OnTargetInSight();


	//~=============================================================================
	// Private
private:
	/** Reference to the controlled pawn, so we don't have to cast everytime. */
	class AEnemyPawn* EnemyPawn;

	/** Keep track of the last waypoint index to be able to send the next. */
	int CurrentWaypointIndex;

};
