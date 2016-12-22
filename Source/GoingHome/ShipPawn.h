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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* FirstPersonCameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* ThirdPersonCameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
	float ProjectileBaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons", meta = (AllowPrivateAccess = "true", MustImplement = "Projectile"))
	TSubclassOf<class AActor> ProjectileClass;

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

public:
	UFUNCTION()
	void OverlapHandler(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	void ActivateThrusterPair(UPhysicsThrusterComponent* one, UPhysicsThrusterComponent* two, float value, float force);
};
