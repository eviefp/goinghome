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

	EnemyRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("EnemyRootComponent"));
	SetRootComponent(EnemyRootComponent);

	EnemyShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyShipMesh"));

	if (EnemyShipMesh != nullptr)
	{
		EnemyShipMesh->AlwaysLoadOnClient = true;
		EnemyShipMesh->AlwaysLoadOnServer = true;
		EnemyShipMesh->bOwnerNoSee = false;
		EnemyShipMesh->bCastDynamicShadow = true;
		EnemyShipMesh->bAffectDynamicIndirectLighting = true;
		EnemyShipMesh->SetupAttachment(RootComponent);
	}

	EnemySightComponent = CreateDefaultSubobject<UEnemySightComponent>(TEXT("EnemySightComponent"));

	if (EnemySightComponent != nullptr)
	{
		EnemySightComponent->SetupAttachment(RootComponent);
	}

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyAIController::StaticClass();

	_target = FVector::ZeroVector;
	PawnTarget = nullptr;
	ProximityEpsilon = 100;
	CurrentHitPoints = 100;
	GiveUpEngageIfDistanceIsGreaterThan = 10000;
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(GoingHomeEnemyPawn, Log, TEXT("Ctor: RootComponent = %s || EnemyShipMesh = %s "),
		*RootComponent->GetComponentLocation().ToString(),
		*EnemyShipMesh->GetComponentLocation().ToString());

	EnemyController = Cast<AEnemyAIController>(Controller);
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PawnTarget != nullptr)
	{
		_target = PawnTarget->GetActorLocation();

		UE_LOG(GoingHomeEnemyPawn, Log, TEXT("Self %s || Target %s || Distance %f"),
			*RootComponent->GetComponentLocation().ToString(),
			*_target.ToString(),
			(_target - RootComponent->GetComponentLocation()).Size());

		if ((_target - RootComponent->GetComponentLocation()).Size() > GiveUpEngageIfDistanceIsGreaterThan)
		{
			EnemyController->OnGiveUpEngage();
		}
	}

	if(_target == FVector::ZeroVector)
		return;

	// Setup rotation
	//auto newRotation = FQuat::FindBetween(RootComponent->GetComponentLocation(), _target).Rotator();
	
	auto newRotation = FRotationMatrix::MakeFromX(_target - GetActorLocation()).Rotator() - GetActorRotation();
	auto interpolatedRotation = FMath::RInterpConstantTo(FRotator::ZeroRotator, newRotation, DeltaTime, TurnSpeed);

	UE_LOG(GoingHomeEnemyPawn, Log, TEXT("Now: %s || Target: %s || Lerp: %s"),
		*RootComponent->GetComponentRotation().ToString(),
		*newRotation.ToString(),
		*interpolatedRotation.ToString());

	// Setup position
	auto currentLocation = RootComponent->GetComponentLocation();
	auto relativeLocation = _target - currentLocation;
	auto interpolatedMoveVector = FMath::VInterpConstantTo(FVector::ZeroVector, relativeLocation, DeltaTime, MaxSpeed);

	// Do it
	AddActorWorldRotation(interpolatedRotation);
	if((currentLocation - _target).Size() > ProximityEpsilon / 2)
		AddActorWorldOffset(interpolatedMoveVector, false);

	DrawDebugLine(
		GetWorld(),
		RootComponent->GetComponentLocation(),
		RootComponent->GetComponentLocation() + RootComponent->GetForwardVector() * 1000,
		FColor(0, 255, 0),
		false, 0.1, 0,
		30
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
	_target = worldPosition;
	PawnTarget = nullptr;
}

void AEnemyPawn::EngagePawn(APawn* Player)
{
	PawnTarget = Player;
}

float AEnemyPawn::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	CurrentHitPoints -= DamageAmount;
	return DamageAmount;
}
