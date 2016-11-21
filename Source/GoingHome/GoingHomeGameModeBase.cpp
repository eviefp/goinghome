// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "GoingHomeGameModeBase.h"
#include "ShipPawn.h"

AGoingHomeGameModeBase::AGoingHomeGameModeBase() : Super()
{
	DefaultPawnClass = AShipPawn::StaticClass();
}


