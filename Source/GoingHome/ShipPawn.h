// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GoingHomeGameState.h"
#include "PawnBase.h"
#include "ShipPawn.generated.h"


UCLASS()
class GOINGHOME_API AShipPawn : public APawnBase
{
	GENERATED_BODY()

	UPROPERTY()
	AGoingHomeGameState* _gameState;

public:
	//~=============================================================================
	// Constructors and overrides.
	AShipPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;


protected:
	//~=============================================================================
	// Components

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UPhysicsThrusterComponent *PitchUp, *PitchDown, *YawLeft, *YawRight, *RollLeft, *RollRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters", meta = (AllowPrivateAccess = "true"))
	float PitchForce;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters", meta = (AllowPrivateAccess = "true"))
	float RollForce;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters", meta = (AllowPrivateAccess = "true"))
	float YawForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters", meta = (AllowPrivateAccess = "true"))
	float ThrustForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* FirstPersonCameraArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* ThirdPersonCameraArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCamera;

private:
	//~=============================================================================
	// Keyboard handlers

	void PitchHandler(float value);
	void YawHandler(float value);
	void RollHandler(float value);
	void ThrustHandler(float value);
	void CycleCameras();

	void ActivateThrusterPair(UPhysicsThrusterComponent* one, UPhysicsThrusterComponent* two, float value, float force);
};
