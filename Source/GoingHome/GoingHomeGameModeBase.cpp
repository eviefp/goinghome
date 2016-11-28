// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "GoingHomeGameModeBase.h"
#include "GoingHomeGameState.h"

AGoingHomeGameModeBase::AGoingHomeGameModeBase() : Super()
{
	GameStateClass = AGoingHomeGameState::StaticClass();
}


