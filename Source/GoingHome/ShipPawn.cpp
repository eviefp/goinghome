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
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;

	this->AutoPossessPlayer = EAutoReceiveInput::Player0;
	this->AutoReceiveInput = EAutoReceiveInput::Player0;

	ShipMesh->SetSimulatePhysics(true);
	ShipMesh->SetEnableGravity(false);
	ShipMesh->SetLinearDamping(0.15);
	ShipMesh->SetAngularDamping(0.4);

	FirstPersonCameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FirstPersonCameraArm"));
	FirstPersonCameraArm->SetupAttachment(ShipMesh);
	FirstPersonCameraArm->AddLocalOffset(FVector(0, -4.6f, -4.96f));
	FirstPersonCameraArm->bEnableCameraLag = true;
	FirstPersonCameraArm->bEnableCameraRotationLag = true;
	FirstPersonCameraArm->TargetArmLength = 0;
	FirstPersonCameraArm->CameraLagSpeed = 50;
	FirstPersonCameraArm->CameraRotationLagSpeed = 5;
	FirstPersonCameraArm->CameraLagMaxDistance = 1;
	FirstPersonCameraArm->bUsePawnControlRotation = true;
	FirstPersonCameraArm->bDoCollisionTest = false;
	FirstPersonCameraArm->bUsePawnControlRotation = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->bAutoActivate = true;
	FirstPersonCamera->SetupAttachment(FirstPersonCameraArm);

	ThirdPersonCameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonCameraArm"));
	ThirdPersonCameraArm->SetupAttachment(ShipMesh);
	ThirdPersonCameraArm->AddLocalOffset(FVector(-500, 0, 0));
	ThirdPersonCameraArm->AddLocalRotation(FRotator(0, -20, 0));
	ThirdPersonCameraArm->bEnableCameraLag = true;
	ThirdPersonCameraArm->bEnableCameraRotationLag = true;
	ThirdPersonCameraArm->TargetArmLength = 500;
	ThirdPersonCameraArm->CameraLagSpeed = 50;
	ThirdPersonCameraArm->CameraRotationLagSpeed = 5;
	ThirdPersonCameraArm->CameraLagMaxDistance = 1;
	ThirdPersonCameraArm->bUsePawnControlRotation = true;
	ThirdPersonCameraArm->bDoCollisionTest = true;
	ThirdPersonCameraArm->bUsePawnControlRotation = false;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->bAutoActivate = false;
	ThirdPersonCamera->SetupAttachment(ThirdPersonCameraArm);

	PitchForce = YawForce = RollForce = 1200;
	ThrustForce = 2000;
}

void AShipPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(InputComponent);

	PlayerInputComponent->BindAxis("Pitch", this, &AShipPawn::PitchHandler);
	PlayerInputComponent->BindAxis("Yaw", this, &AShipPawn::YawHandler);
	PlayerInputComponent->BindAxis("Roll", this, &AShipPawn::RollHandler);
	PlayerInputComponent->BindAxis("Thrust", this, &AShipPawn::ThrustHandler);

	PlayerInputComponent->BindAction("CycleCamera", IE_Pressed, this, &AShipPawn::CycleCameras);


	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AShipPawn::BeginPlay()
{
	Super::BeginPlay();
	_gameState = Cast<AGoingHomeGameState>(GetWorld()->GetGameState());
}

void AShipPawn::Tick(float delta)
{
	Super::Tick(delta);
}

void AShipPawn::PitchHandler(float value)
{
	ShipMesh->AddAngularImpulse(ShipMesh->GetRightVector() * (PitchForce*value));
	_gameState->OnPitch(value);
}

void AShipPawn::YawHandler(float value)
{
	ShipMesh->AddAngularImpulse(ShipMesh->GetUpVector() * (YawForce*value));
	_gameState->OnYaw(value);
}

void AShipPawn::RollHandler(float value)
{
	ShipMesh->AddAngularImpulse(ShipMesh->GetForwardVector() * (RollForce*value));
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
