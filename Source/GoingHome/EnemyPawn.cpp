// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "EnemyPawn.h"
#include "EnemyAIController.h"
#include "EnemySightComponent.h"
#include "Projectile.h"
#include "GoingHomeWorldSettings.h"
#include "Algo/Reverse.h"

// Sets default values
AEnemyPawn::AEnemyPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UClass> ProjectileBlueprintClassFinder(TEXT("Class'/Game/Blueprints/BP_ShipProjectile.BP_ShipProjectile_C'"));

	if (ProjectileBlueprintClassFinder.Object != nullptr)
	{
		ProjectileBlueprintClass = ProjectileBlueprintClassFinder.Object;
	}

	EnemyShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyShipMesh"));

	if (EnemyShipMesh != nullptr)
	{
		EnemyShipMesh->AlwaysLoadOnClient = true;
		EnemyShipMesh->AlwaysLoadOnServer = true;
		EnemyShipMesh->bOwnerNoSee = false;
		EnemyShipMesh->bCastDynamicShadow = true;
		EnemyShipMesh->bAffectDynamicIndirectLighting = true;
		SetRootComponent(EnemyShipMesh);
	}

	EnemySightComponent = CreateDefaultSubobject<UEnemySightComponent>(TEXT("EnemySightComponent"));

	if (EnemySightComponent != nullptr)
	{
		EnemySightComponent->SetupAttachment(EnemyShipMesh);
	}

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyAIController::StaticClass();

	_target = FVector::ZeroVector;
	PawnTarget = nullptr;
	ProximityEpsilon = 100;
	CurrentHitPoints = 100;
	RateOfFire = 1;
	GiveUpEngageIfDistanceIsGreaterThan = 10000;
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(GoingHomeEnemyPawn, Log, TEXT("Ctor: RootComponent = %s || EnemyShipMesh = %s "),
		*RootComponent->GetComponentLocation().ToString(),
		*EnemyShipMesh->GetComponentLocation().ToString()
	);

	EnemyController = Cast<AEnemyAIController>(Controller);
	LastTimeWeShot = -100;

	InitialiseNavigationManager();
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If we're engaging, set the target to the enemy's current location.
	if (PawnTarget != nullptr)
		_target = PawnTarget->GetActorLocation();

	// If target is 0, 0, 0 do nothing. This means we have a bug that we can't ever move to origin, exactly.
	if (_target == FVector::ZeroVector)
		return;

	// Target location debug point.
	//DrawDebugSphere(
	//	GetWorld(),
	//	_target,
	//	50,
	//	32,
	//	FColor(255, 0, 0),
	//	false,
	//	0.1
	//);

	// Make a rotation from the distance vector, get the delta rotation and interpolate it
	auto newRotation = FRotationMatrix::MakeFromX(_target - GetActorLocation()).Rotator();
	auto deltaRotation = newRotation - GetActorRotation();
	auto interpolatedRotation = FMath::RInterpConstantTo(FRotator::ZeroRotator, deltaRotation, DeltaTime, TurnSpeed);

	// Interpolate the relative move vector
	auto currentLocation = RootComponent->GetComponentLocation();
	auto deltaLocation = _target - currentLocation;
	auto interpolatedMoveVector = FMath::VInterpConstantTo(FVector::ZeroVector, deltaLocation, DeltaTime, MaxSpeed);

	// Don't move or rotate if we're too close
	if ((currentLocation - _target).Size() > ProximityEpsilon / 2)
	{
		AddActorLocalRotation(interpolatedRotation);
		AddActorWorldOffset(interpolatedMoveVector, false);
	}

	// Forward vector debug line.
	//DrawDebugLine(
	//	GetWorld(),
	//	RootComponent->GetComponentLocation(),
	//	RootComponent->GetComponentLocation() + RootComponent->GetForwardVector() * 3000,
	//	FColor(0, 255, 0),
	//	false, 0.1, 0,
	//	30
	//);

	// If we are currently engaging a target, give up if distance is too big.
	if (PawnTarget != nullptr)
	{
		if (deltaRotation.IsNearlyZero(0.1))
			Shoot();

		if (deltaLocation.Size() > GiveUpEngageIfDistanceIsGreaterThan)
		{
			UE_LOG(GoingHomeEnemyPawn, Log, TEXT("Giving up engage at Self %s || Target %s || Distance %f"),
				*RootComponent->GetComponentLocation().ToString(),
				*_target.ToString(),
				deltaLocation.Size()
			);

			EnemyController->OnGiveUpEngage();
		}
	}

	// Otherwise, if we're not engaging and we're close enough, reset movement and notify controller.
	else if (PawnTarget == nullptr && (currentLocation - _target).Size() < ProximityEpsilon)
	{
		if (PathSolutionWaypoints.Num() > 0)
			_target = PathSolutionWaypoints.Pop();
		else
		{
			_target = FVector::ZeroVector;
			EnemyController->OnTargetReached();
		}
	}
}

void AEnemyPawn::MoveTo(FVector worldPosition)
{
	InitialiseNavigationManager();

	// defaults will do
	FDoNNavigationQueryParams queryParams; 
	queryParams.bPreciseDynamicCollisionRepathing = true;
	queryParams.bFlexibleOriginGoal = true;

	// all the debugging
	FDoNNavigationDebugParams debugParams;
	debugParams.VisualizeInRealTime = debugParams.VisualizeOptimizedPath = debugParams.VisualizeRawPath = true;

	FDoNNavigationResultHandler resultHandler;
	resultHandler.BindDynamic(this, &AEnemyPawn::NavigationQuqeryHandler);

	FDonNavigationDynamicCollisionDelegate collisionDelegate;
	collisionDelegate.BindDynamic(this, &AEnemyPawn::DynamicNavigationQuqeryHandler);

	if (!NavigationManager->SchedulePathfindingTask(this, worldPosition, queryParams, debugParams, resultHandler, collisionDelegate))
	{
		FTimerDelegate retryDelegate = FTimerDelegate::CreateUObject(this, &AEnemyPawn::MoveTo, worldPosition);
		GetWorldTimerManager().SetTimer(PathRetryTimerHandle, retryDelegate, 5, false);
	}

	PawnTarget = nullptr;
}

void AEnemyPawn::EngagePawn(APawn* Player)
{
	PawnTarget = Player;
}

float AEnemyPawn::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	CurrentHitPoints -= DamageAmount;

	UE_LOG(GoingHomeEnemyPawn, Log, TEXT("Took damage from %s. I have %d HP left."), 
		*DamageCauser->GetName(),
		CurrentHitPoints
	);

	return DamageAmount;
}

void AEnemyPawn::Shoot()
{
	auto timeNow = GetWorld()->GetTimeSeconds();

	if ((timeNow - LastTimeWeShot) * RateOfFire < 1)
		return;

	LastTimeWeShot = timeNow;

	// current transform
	auto transform = GetActorTransform();

	// should be in front so we don't get collisions
	transform.SetLocation(transform.GetLocation() + GetActorForwardVector() * 100);

	//Spawn
	FActorSpawnParameters spawnParams;
	spawnParams.Instigator = this;
	spawnParams.Owner = this;
	
	auto projectile = GetWorld()->SpawnActor<AActor>(ProjectileBlueprintClass, transform, spawnParams);
	if (projectile != nullptr)
	{
		IProjectile::Execute_SetProjectileParent(projectile, this);
		IProjectile::Execute_SetLinearVelocity(projectile, GetActorForwardVector() * 50000);
	}
}

void AEnemyPawn::InitialiseNavigationManager()
{
	if (NavigationManager != nullptr)
		return;

	auto worldSettings = Cast<AGoingHomeWorldSettings>(GetWorld()->GetWorldSettings());

	if (worldSettings != nullptr)
		NavigationManager = worldSettings->DonNavigationManager;
}

void AEnemyPawn::NavigationQuqeryHandler(const FDoNNavigationQueryData& data)
{
	PathSolutionWaypoints = TArray<FVector>();
	for (auto i = data.PathSolutionOptimized.Num() - 1; i >= 0; --i)
	{
		PathSolutionWaypoints.Push(data.PathSolutionOptimized[i]);
	}

	if (PathSolutionWaypoints.Num())
		_target = PathSolutionWaypoints.Pop();
}

void AEnemyPawn::DynamicNavigationQuqeryHandler(const struct FDonNavigationDynamicCollisionPayload& data)
{
}
