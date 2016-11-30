// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "EnemyPawn.h"
#include "EnemyAIController.h"
#include "EnemySightComponent.h"

// Sets default values
AEnemyPawn::AEnemyPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyShipMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("EnemyShipMesh"));
	EnemySightComponent = ObjectInitializer.CreateDefaultSubobject<UEnemySightComponent>(this, TEXT("EnemySightComponent"));

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyAIController::StaticClass();

	_target = FVector::ZeroVector;
	PawnTarget = nullptr;
	ProximityEpsilon = 100;
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	EnemyController = Cast<AEnemyAIController>(Controller);
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PawnTarget != nullptr)
		_target = PawnTarget->GetActorLocation();

	if(_target == FVector::ZeroVector)
		return;

	// Setup rotation
	auto newRotation = RootComponent->GetComponentRotation() - (RootComponent->GetComponentLocation() - _target).Rotation();
	auto interpolatedRotation = FMath::RInterpConstantTo(FRotator::ZeroRotator, newRotation, DeltaTime, TurnSpeed);

	// Setup position
	auto currentLocation = RootComponent->GetComponentLocation();
	auto relativeLocation = _target - currentLocation;
	auto interpolatedMoveVector = FMath::VInterpConstantTo(FVector::ZeroVector, relativeLocation, DeltaTime, MaxSpeed);

	// Do it
	if((currentLocation - _target).Size() > ProximityEpsilon / 2)
		AddActorWorldTransform(FTransform(interpolatedRotation, interpolatedMoveVector), false);

	DrawDebugLine(
		GetWorld(),
		RootComponent->GetComponentLocation(),
		RootComponent->GetComponentLocation() + RootComponent->GetForwardVector() * 1000,
		FColor(0, 255, 0),
		false, -1, 0,
		12.333
	);
	
	// Is it over yet?
	if(PawnTarget == nullptr && (currentLocation - _target).Size() < ProximityEpsilon)
	{
		_target = FVector::ZeroVector;
		EnemyController->OnTargetReached();
	}
}

void AEnemyPawn::MoveTo(FVector worldPosition)
{
	UE_LOG(GoingHomeEnemyPawn, Log, TEXT("%s moving to %s"), *GetName() ,*worldPosition.ToString());
	_target = worldPosition;
	PawnTarget = nullptr;
}

void AEnemyPawn::EngagePawn(APawn* Player)
{
	PawnTarget = Player;
}
