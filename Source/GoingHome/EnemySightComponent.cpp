// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "EnemySightComponent.h"
#include "EnemyPawn.h"
#include "EnemyAIController.h"

FName UEnemySightComponent::SightRangeName(TEXT("SightRange"));

// Sets default values for this component's properties
UEnemySightComponent::UEnemySightComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SightRange = 1000;

	SightCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("SightCollisionComponent"));

	if (SightCollisionComponent != nullptr)
	{
		SightCollisionComponent->AlwaysLoadOnClient = true;
		SightCollisionComponent->AlwaysLoadOnServer = true;
		SightCollisionComponent->bOwnerNoSee = false;
		SightCollisionComponent->bCastDynamicShadow = false;
		SightCollisionComponent->bAffectDynamicIndirectLighting = true;
		SightCollisionComponent->SetHiddenInGame(false);
		SightCollisionComponent->SetBoxExtent(FVector(SightRange * 2));
		SightCollisionComponent->SetupAttachment(this);
	}

	
}


void UEnemySightComponent::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	auto propertyName = PropertyChangedEvent.Property != nullptr ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (propertyName == SightRangeName)
	{
		UE_LOG(GoingHomeEnemySightComponent, Log, TEXT("Property changed to %f"), SightRange * 2);
		SightCollisionComponent->SetBoxExtent(FVector(SightRange * 2));
	}

	UE_LOG(GoingHomeEnemySightComponent, Log, TEXT("Property changed %s"), *propertyName.ToString());
}

// Called when the game starts
void UEnemySightComponent::BeginPlay()
{
	Super::BeginPlay();

	isOverlapping = false;
	
	SightCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &UEnemySightComponent::OnBeginOverlap);
	SightCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &UEnemySightComponent::OnEndOverlap);

	EnemyPawn = Cast<AEnemyPawn>(GetOwner());
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	EnemyController = Cast<AEnemyAIController>(EnemyPawn->GetController());
}


// Called every frame
void UEnemySightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (isOverlapping)
	{
		auto playerLocation = PlayerPawn->GetActorLocation();
		auto rootComponent = EnemyPawn->GetRootComponent();
		auto myTransform = rootComponent->GetComponentTransform();

		auto rotation = rootComponent->GetComponentRotation() - (myTransform.GetLocation() - playerLocation).Rotation();
		
		if (rotation.Roll <= 90 && rotation.Pitch <= 90 && rotation.Yaw <= 90)
		{
			UE_LOG(GoingHomeEnemySightComponent, Log, TEXT("Pawn seen: %s"), *PlayerPawn->GetName());
			EnemyController->OnTargetInSight();
			isOverlapping = false;
		}
	}
}

void UEnemySightComponent::OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Cast<APawn>(OtherActor) != nullptr) 
	{
		UE_LOG(GoingHomeEnemySightComponent, Log, TEXT("Pawn is overlapping: %s"), *OtherActor->GetName());
		isOverlapping = true;
	}
}

void UEnemySightComponent::OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<APawn>(OtherActor) != nullptr)
	{
		UE_LOG(GoingHomeEnemySightComponent, Log, TEXT("Pawn left overlap area: %s"), *OtherActor->GetName());
		isOverlapping = false;
	}
}

