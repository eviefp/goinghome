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

	UE_LOG(GoingHomeEnemyAIController, Log, TEXT("Moving to location #%d."), CurrentWaypointIndex);
	EnemyPawn->MoveTo(EnemyPawn->Waypoints[CurrentWaypointIndex]);
}

void AEnemyAIController::OnGiveUpEngage()
{
	UE_LOG(GoingHomeEnemyAIController, Log, TEXT("Enemy giving up engage."));
	OnTargetReached();
}

void AEnemyAIController::OnTargetInSight()
{
	auto pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	UE_LOG(GoingHomeEnemyAIController, Log, TEXT("Engaging pawn: %s."), *pawn->GetName());
	EnemyPawn->EngagePawn(pawn);
}
