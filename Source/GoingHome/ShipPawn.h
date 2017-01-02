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

	virtual void Tick(float delta) override;


protected:
	//~=============================================================================
	// Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters", meta = (AllowPrivateAccess = "true"))
	float PitchForce;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters", meta = (AllowPrivateAccess = "true"))
	float RollForce;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters", meta = (AllowPrivateAccess = "true"))
	float YawForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters", meta = (AllowPrivateAccess = "true"))
	float ThrustForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cameras", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* FirstPersonCameraArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cameras", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cameras", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* ThirdPersonCameraArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cameras", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
	float ProjectileBaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons", meta = (AllowPrivateAccess = "true", MustImplement = "Projectile"))
	TSubclassOf<class AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	class URadarComponent* RadarComponent;

private:
	//~=============================================================================
	// Keyboard handlers

	void PitchHandler(float value);
	void YawHandler(float value);
	void RollHandler(float value);
	void ThrustHandler(float value);
	void CycleCameras();
	void ShootHandler();
	void MineHandler(float value);
	void ToggleMouseLookOn();
	void ToggleMouseLookOff();

	bool MouseLookEnabled;
	class UCameraComponent* CurrentCamera;

public:
	UFUNCTION()
	void OverlapHandler(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
