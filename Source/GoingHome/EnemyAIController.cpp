// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "EnemyAIController.h"
#include "EnemyPawn.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void AEnemyAIController::BeginPlay()
{
	auto* pawn = GetPawn();

	EnemyPawn = Cast<AEnemyPawn>(pawn);
	CurrentWaypointIndex = -1;

	OnTargetReached();
}

void AEnemyAIController::OnTargetReached()
{
	if (EnemyPawn->Waypoints.Num() <= 0) 
	{
		UE_LOG(GoingHomeEnemyAIController, Log, TEXT("No Waypoints found. Doing nothing."));
		return;
	}

	++CurrentWaypointIndex;

	if (CurrentWaypointIndex == EnemyPawn->Waypoints.Num())
		CurrentWaypointIndex = 0;

	EnemyPawn->MoveTo(EnemyPawn->Waypoints[CurrentWaypointIndex]);
}

void AEnemyAIController::OnTargetInSight()
{
	auto pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	EnemyPawn->EngagePawn(pawn);
}
