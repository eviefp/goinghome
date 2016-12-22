// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "GoingHomeWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class GOINGHOME_API AGoingHomeWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> BeaconWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> MainPlayerHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	class ADonNavigationManager* DonNavigationManager;

};
