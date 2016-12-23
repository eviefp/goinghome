// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "UMG.h"
#include "GoingHomeWorldSettings.h"
#include "HumanPlayerController.h"


void AHumanPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto worldSettings = Cast<AGoingHomeWorldSettings>(GetWorld()->GetWorldSettings());

	if (worldSettings != nullptr)
	{
		auto hudWidget = worldSettings->MainPlayerHUD;
		if (hudWidget != nullptr)
		{
			auto widget = CreateWidget<UUserWidget>(this, hudWidget);

			if (widget)
			{
				widget->AddToViewport();
			}
		}
	}
}

