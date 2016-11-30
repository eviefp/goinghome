// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "EnemySightComponent.h"
#include "EnemyPawn.h"
#include "EnemyAIController.h"

// Sets default values for this component's properties
UEnemySightComponent::UEnemySightComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SightCollisionComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("EnemySightBoxCollisionComponent"));
	SightRange = 1000;
}


// Called when the game starts
void UEnemySightComponent::BeginPlay()
{
	Super::BeginPlay();

	isOverlapping = false;
	
	SightCollisionComponent->SetHiddenInGame(false);
	SightCollisionComponent->SetBoxExtent(FVector(SightRange* 2));
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
			EnemyController->OnTargetInSight();
			isOverlapping = false;
		}
	}
}

void UEnemySightComponent::OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Cast<APawn>(OtherActor) != nullptr) 
	{
		isOverlapping = true;
	}
}

void UEnemySightComponent::OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<APawn>(OtherActor) != nullptr)
	{
		isOverlapping = false;
	}
}

