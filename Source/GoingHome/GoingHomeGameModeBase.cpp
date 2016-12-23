// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "GoingHomeGameModeBase.h"
#include "GoingHomeGameState.h"
#include "HumanPlayerController.h"

AGoingHomeGameModeBase::AGoingHomeGameModeBase() : Super()
{
	GameStateClass = AGoingHomeGameState::StaticClass();
	PlayerControllerClass = AHumanPlayerController::StaticClass();
}


