// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "ShipPawn.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Projectile.h"
#include "Mineable.h"
#include "RadarComponent.h"

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
	FirstPersonCameraArm->AddLocalOffset(FVector(-4.0f, -32.6f, 1.04f));
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

	RadarComponent = CreateDefaultSubobject<URadarComponent>(TEXT("RadarComponent"));
	RadarComponent->SetupAttachment(ShipMesh);
	RadarComponent->SetRelativeLocation(FVector(26.0f, -33.01358f, -8.8f));

	PitchForce = YawForce = 1200;
	RollForce = 2400;
	ThrustForce = 2000;
	ProjectileBaseSpeed = 10000;
	MouseLookEnabled = false;
	CurrentCamera = FirstPersonCamera;

	RootComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
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

	PlayerInputComponent->BindAction("MouseLook", IE_Pressed, this, &AShipPawn::ToggleMouseLookOn);
	PlayerInputComponent->BindAction("MouseLook", IE_Released, this, &AShipPawn::ToggleMouseLookOff);

	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AShipPawn::BeginPlay()
{
	Super::BeginPlay();

	ShipMesh->OnComponentBeginOverlap.AddDynamic(this, &AShipPawn::OverlapHandler);

	_gameState = Cast<AGoingHomeGameState>(GetWorld()->GetGameState());
}

void AShipPawn::Tick(float delta)
{
	Super::Tick(delta);

	if (!MouseLookEnabled && !CurrentCamera->RelativeRotation.IsZero())
	{
		CurrentCamera->SetRelativeRotation(FMath::Lerp(CurrentCamera->RelativeRotation, FRotator(0, 0, 0), 0.05f));
	}
}

void AShipPawn::PitchHandler(float value)
{
	if (MouseLookEnabled)
	{
		CurrentCamera->AddRelativeRotation(FRotator(-value, 0, 0));
		return;
	}

	ShipMesh->AddAngularImpulse(ShipMesh->GetRightVector() * (PitchForce*value));
	_gameState->OnPitch(value);
}

void AShipPawn::YawHandler(float value)
{
	if (MouseLookEnabled)
	{
		CurrentCamera->AddRelativeRotation(FRotator(0, value, 0));
		return;
	}

	ShipMesh->AddAngularImpulse(ShipMesh->GetUpVector() * (YawForce*value));
	_gameState->OnYaw(value);
}

void AShipPawn::RollHandler(float value)
{
	if (MouseLookEnabled)
	{
		return;
	}

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
		CurrentCamera = ThirdPersonCamera;
		return;
	}

	ThirdPersonCamera->Deactivate();
	FirstPersonCamera->Activate();
	CurrentCamera = FirstPersonCamera;
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
		IProjectile::Execute_SetLinearVelocity(projectile, CurrentCamera->GetForwardVector() * ProjectileBaseSpeed);
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

void AShipPawn::ToggleMouseLookOn()
{
	MouseLookEnabled = true;
}

void AShipPawn::ToggleMouseLookOff()
{
	MouseLookEnabled = false;
}
