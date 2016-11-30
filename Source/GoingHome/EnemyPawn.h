// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "EnemyPawn.generated.h"

/**
 * EnemyPawn has a mesh and a sight component as well as the array of Waypoints. 
 * It takes MoveTo orders from EnemyAIController.
 */
UCLASS()
class GOINGHOME_API AEnemyPawn : public APawn
{
	GENERATED_BODY()

public:
	//~=============================================================================
	// Constructors and overrides.

	/** Initializes the StaticMesh.*/
	AEnemyPawn(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;


	//~=============================================================================
	// Visual Components
	
	/** The ship mesh. Not sure why I can't edit the texture. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	class UStaticMeshComponent* EnemyShipMesh;


	//~=============================================================================
	// Properties

	/** Maximum speed used in constant interpolation of transpose movement. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxSpeed;

	/** Turn speed used in constant interpolation of rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TurnSpeed;

	/** This is close enough, don't really have to patrol to exact coordinates. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ProximityEpsilon;

	/** Not sure why we're keeping a pointer to this after creating it. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UEnemySightComponent* EnemySightComponent;


	//~=============================================================================
	// AI Behaviour

	/** List of waypoints that are patrolled by AI. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<struct FVector> Waypoints;


	//~=============================================================================
	// Actions

	/** 
	 * Called by EnemyAIController with the next position. It is a bit odd, since the
	 * Waypoints are stored here, isn't it?
	 */
	virtual void MoveTo(struct FVector worldPosition);

	virtual void EngagePawn(class APawn* Player);


	//~=============================================================================
	// Private

private:
	/** Controller reference so we don't cast GetController() everytime. */
	class AEnemyAIController* EnemyController;

	/** We need to keep track of where we're going. */
	struct FVector _target;

	/**  */
	class APawn* PawnTarget;

};
