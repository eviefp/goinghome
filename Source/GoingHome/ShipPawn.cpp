// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "ShipPawn.h"
#include "GoingHomeWorldSettings.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsThrusterComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "UMG.h"
#include "Projectile.h"
#include "Mineable.h"

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
	ShipMesh->bGenerateOverlapEvents = true;

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
	ProjectileBaseSpeed = 10000;
}

void AShipPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(InputComponent);

	PlayerInputComponent->BindAxis("Pitch", this, &AShipPawn::PitchHandler);
	PlayerInputComponent->BindAxis("Yaw", this, &AShipPawn::YawHandler);
	PlayerInputComponent->BindAxis("Roll", this, &AShipPawn::RollHandler);
	PlayerInputComponent->BindAxis("Thrust", this, &AShipPawn::ThrustHandler);
	PlayerInputComponent->BindAxis("Mine", this, &AShipPawn::MineHandler);

	PlayerInputComponent->BindAction("CycleCamera", IE_Pressed, this, &AShipPawn::CycleCameras);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AShipPawn::ShootHandler);

	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AShipPawn::BeginPlay()
{
	Super::BeginPlay();

	ShipMesh->OnComponentBeginOverlap.AddDynamic(this, &AShipPawn::OverlapHandler);

	_gameState = Cast<AGoingHomeGameState>(GetWorld()->GetGameState());

	// todo: create and move to player controller
	auto worldSettings = Cast<AGoingHomeWorldSettings>(GetWorld()->GetWorldSettings());

	if (worldSettings != nullptr)
	{
		auto hudWidget = worldSettings->MainPlayerHUD;
		if (hudWidget != nullptr)
		{
			//auto widget = CreateWidget<UUserWidget>()
			
		/*	if (widget)
			{
				widget->AddToViewport();
			}*/
		}
	}
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

void AShipPawn::ShootHandler()
{
	if (ProjectileClass == nullptr)
		return;

	// current transform
	auto transform = GetActorTransform();

	// should be in front so we don't get collisions
	transform.SetLocation(transform.GetLocation() + GetActorForwardVector() * 100);

	//Spawn
	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = this;
	spawnParams.Owner = this;

	auto projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, transform, spawnParams);
	if (projectile != nullptr)
	{
		IProjectile::Execute_SetProjectileParent(projectile, this);
		IProjectile::Execute_SetLinearVelocity(projectile, GetActorForwardVector() * ProjectileBaseSpeed);
	}
}

void AShipPawn::OverlapHandler(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	_gameState->OnOverlap(OtherActor->GetFName());
}

void AShipPawn::MineHandler(float value)
{
	if (value <= 0)
		return;

	FHitResult result;
	FCollisionQueryParams queryParams;

	queryParams.AddIgnoredActor(this);
	queryParams.bTraceComplex = false;
	
	if (GetWorld()->LineTraceSingleByChannel(result, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 10000, ECollisionChannel::ECC_Visibility, queryParams))
	{
		if (result.GetActor()->GetClass()->ImplementsInterface(UMineable::StaticClass()))
		{
			_gameState->OnMine(result.GetActor()->GetFName());
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, "TODO: mining code");
		}
	}
}
