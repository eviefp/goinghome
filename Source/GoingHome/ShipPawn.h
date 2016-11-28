// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GoingHomeGameState.h"
#include "GameFramework/Pawn.h"
#include "ShipPawn.generated.h"


UCLASS()
class GOINGHOME_API AShipPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY()
	AGoingHomeGameState* _gameState;

public:
	// Sets default values for this pawn's properties
	AShipPawn();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Events")
	void OnPitch(float value);
	
};
