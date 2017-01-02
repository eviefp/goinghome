// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "RadarComponent.h"
#include "Components/SphereComponent.h"

FName URadarComponent::RadarMaterialName(TEXT("RadarMaterial"));

URadarComponent::URadarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

	SetCollisionProfileName("NoCollision");

	SetRelativeScale3D(FVector(0.05f, 0.05f, 0.05f));

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(this);
	DetectionSphere->InitSphereRadius(1000.0f);
	DetectionSphere->SetRelativeScale3D(FVector(100.0f, 100.0f, 100.0f));
}

void URadarComponent::BeginPlay()
{
	Super::BeginPlay();

	DetectionSphere->RegisterComponent();

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &URadarComponent::OnBeginOverlap);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &URadarComponent::OnEndOverlap);
}

void URadarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto distanceFactor = Bounds.GetSphere().W / DetectionSphere->GetScaledSphereRadius();

	DrawDebugSphere(
		GetWorld(),
		GetOwner()->GetActorLocation(),
		DetectionSphere->GetScaledSphereRadius(),
		16,
		FColor::Blue,
		false,
		1.0f,
		0,
		1.0f
	);

	UE_LOG(GoingHomeRadar, Log, TEXT("Distance factor is %f"), distanceFactor);

	for (auto otherActor : OverlappingActors)
	{
		auto distance = otherActor.Key->GetActorLocation() - GetOwner()->GetActorLocation();

		DrawDebugLine(
			GetWorld(),
			GetOwner()->GetActorLocation(),
			GetOwner()->GetActorLocation() + distance,
			FColor::Blue,
			false,
			1.0f,
			0,
			1.0f
		);

		DrawDebugLine(
			GetWorld(),
			GetOwner()->GetActorLocation(),
			GetOwner()->GetActorLocation() + (distance * distanceFactor * 10),
			FColor::Red,
			false,
			1.0f,
			0,
			1.0f
		);

		otherActor.Value->SetRelativeLocation(distance * distanceFactor, false);
	}
}

void URadarComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	FString name = "RadarComponentItem-";
	name += OtherActor->GetFName().ToString();

	auto radarItem = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), GetOwner(), FName(*name));
	radarItem->SetCollisionProfileName("NoCollision");
	radarItem->OnComponentCreated();
	radarItem->SetStaticMesh(RadarEntityMesh);
	radarItem->SetWorldScale3D(FVector(0.0015f, 0.0015f, 0.0015f));
	
	radarItem->RegisterComponent();

	radarItem->AttachTo(DetectionSphere);

	OverlappingActors.Add(OtherActor, radarItem);

	UE_LOG(GoingHomeRadar, Log, TEXT("(%d) Adding actor %s to radar using item %s"), OverlappingActors.Num(), *OtherActor->GetName(), *radarItem->GetName());
}

void URadarComponent::OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(GoingHomeRadar, Log, TEXT("Removing %s from radar."), *OtherActor->GetName());
	auto radarItem = *OverlappingActors.Find(OtherActor);
	
	radarItem->DestroyComponent(false);

	OverlappingActors.Remove(OtherActor);

}
