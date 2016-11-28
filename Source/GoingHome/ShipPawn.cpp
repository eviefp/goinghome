// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "ShipPawn.h"


// Sets default values
AShipPawn::AShipPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AShipPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AShipPawn::BeginPlay()
{
	_gameState = Cast<AGoingHomeGameState>(GetWorld()->GetGameState());
}

void AShipPawn::OnPitch(float value)
{
	_gameState->OnPitch(value);
}


