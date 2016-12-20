// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "ShipPawn.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsThrusterComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"


// Sets default values
AShipPawn::AShipPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->AutoPossessPlayer = EAutoReceiveInput::Player0;
	this->AutoReceiveInput = EAutoReceiveInput::Player0;

	PitchUp = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("PitchUpThruster"));
	PitchUp->SetupAttachment(ShipMesh);
	PitchUp->AddLocalOffset(FVector(0, 0, 400));
	PitchUp->AddLocalRotation(FRotator(0, 0, 90));

	PitchDown = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("PitchDownThruster"));
	PitchDown->SetupAttachment(ShipMesh);
	PitchDown->AddLocalOffset(FVector(0, 0, -400));
	PitchDown->AddLocalRotation(FRotator(0, 0, -90));

	YawLeft = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("YawLeftThruster"));
	YawLeft->SetupAttachment(ShipMesh);
	YawLeft->AddLocalOffset(FVector(0, 0, -400));
	YawLeft->AddLocalRotation(FRotator(0, 0, 180));

	YawRight = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("YawRightThruster"));
	YawRight->SetupAttachment(ShipMesh);
	YawRight->AddLocalOffset(FVector(0, 0, 400));

	RollLeft = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("RollLeftThruster"));
	RollLeft->SetupAttachment(ShipMesh);
	RollLeft->AddLocalOffset(FVector(-400, 0, 0));
	RollLeft->AddLocalRotation(FRotator(0, 0, 90));

	RollRight = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("RollRightThruster"));
	RollRight->SetupAttachment(ShipMesh);
	RollRight->AddLocalOffset(FVector(400, 0, 0));
	RollRight->AddLocalRotation(FRotator(0, 0, -90));

	FirstPersonCameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FirstPersonCameraArm"));
	FirstPersonCameraArm->SetupAttachment(ShipMesh);
	FirstPersonCameraArm->AddLocalOffset(FVector(0, -4.6f, -4.96f));
	FirstPersonCameraArm->bEnableCameraLag = true;
	FirstPersonCameraArm->bEnableCameraRotationLag = true;
	FirstPersonCameraArm->TargetArmLength = 1;
	FirstPersonCameraArm->CameraLagSpeed = 100;
	FirstPersonCameraArm->CameraRotationLagSpeed = 5;
	FirstPersonCameraArm->CameraLagMaxDistance = 4;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->bAutoActivate = true;
	FirstPersonCamera->SetupAttachment(FirstPersonCameraArm);

	ThirdPersonCameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonCameraArm"));
	ThirdPersonCameraArm->SetupAttachment(ShipMesh);
	ThirdPersonCameraArm->AddLocalOffset(FVector(-300, 0, 300));
	ThirdPersonCameraArm->AddLocalRotation(FRotator(0, 65, 0));
	ThirdPersonCameraArm->bEnableCameraLag = true;
	ThirdPersonCameraArm->bEnableCameraRotationLag = true;
	ThirdPersonCameraArm->TargetArmLength = 1;
	ThirdPersonCameraArm->CameraLagSpeed = 100;
	ThirdPersonCameraArm->CameraRotationLagSpeed = 5;
	ThirdPersonCameraArm->CameraLagMaxDistance = 4;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->bAutoActivate = false;
	ThirdPersonCamera->SetupAttachment(ThirdPersonCameraArm);

}

void AShipPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(InputComponent);

	PlayerInputComponent->BindAxis("Pitch", this, &AShipPawn::PitchHandler);
	PlayerInputComponent->BindAxis("Yaw", this, &AShipPawn::YawHandler);
	PlayerInputComponent->BindAxis("Roll", this, &AShipPawn::RollHandler);
	PlayerInputComponent->BindAxis("Thrust", this, &AShipPawn::ThrustHandler);

	PlayerInputComponent->BindAction("Camera", IE_Pressed, this, &AShipPawn::CycleCameras);


	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AShipPawn::BeginPlay()
{
	_gameState = Cast<AGoingHomeGameState>(GetWorld()->GetGameState());
}

void AShipPawn::ActivateThrusterPair(UPhysicsThrusterComponent* one, UPhysicsThrusterComponent* two, float value, float force)
{
	if (value == 0)
	{
		one->Deactivate();
		two->Deactivate();
		return;
	}

	one->ThrustStrength = two->ThrustStrength = value * force;

	one->Activate();
	two->Activate();
}

void AShipPawn::PitchHandler(float value)
{
	ActivateThrusterPair(PitchUp, PitchDown, value, PitchForce);
	_gameState->OnPitch(value);
}

void AShipPawn::YawHandler(float value)
{
	ActivateThrusterPair(YawLeft, YawRight, value, YawForce);
	_gameState->OnYaw(value);
}

void AShipPawn::RollHandler(float value)
{
	ActivateThrusterPair(RollLeft, RollRight, value, RollForce);
	_gameState->OnRoll(value);
}

void AShipPawn::ThrustHandler(float value)
{
	auto force = value * ThrustForce;
	auto impulse = ShipMesh->GetForwardVector() * force;
	ShipMesh->AddImpulse(impulse);
	_gameState->OnThrust(value);
}

void AShipPawn::CycleCameras()
{
	if (FirstPersonCamera->IsActive())
	{
		FirstPersonCamera->Deactivate();
		ThirdPersonCamera->Activate();
		return;
	}

	ThirdPersonCamera->Deactivate();
	FirstPersonCamera->Activate();
}
