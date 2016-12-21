// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "PawnBase.generated.h"

UCLASS()
class GOINGHOME_API APawnBase : public APawn
{
	GENERATED_BODY()

public:
	//~=============================================================================
	// Constructors and overrides.

	// Default constructor.
	APawnBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


protected:
	//~=============================================================================
	// Visual Components

	/** The ship mesh and root component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMesh;

};
