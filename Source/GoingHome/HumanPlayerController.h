// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "HumanPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GOINGHOME_API AHumanPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	virtual void BeginPlay() override;
	
};
