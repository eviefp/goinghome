// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "Radar.h"


// Sets default values
ARadar::ARadar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Sphere
	RadarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RadarMesh"));
	SetRootComponent(RadarMesh);
	RootComponent = RadarMesh;

	RadarMesh->SetCollisionProfileName("NoCollision");

	ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (mesh.Succeeded())
	{
		RadarMesh->SetStaticMesh(mesh.Object);
	}

	ConstructorHelpers::FObjectFinder<UMaterial> material(TEXT("Material'/Game/Materials/WireframeMaterial.WireframeMaterial'"));
	if (material.Succeeded())
	{
		RadarMesh->SetMaterial(0, UMaterialInstanceDynamic::Create(material.Object, RadarMesh));
	}
}

// Called when the game starts or when spawned
void ARadar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARadar::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	FVector origin, extends;


	//DrawDebugPoint(GetWorld(), GetActorLocation() + FVector(20, 20, 20), 15, FColor::Red, false, DeltaTime*1.1);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s"), *RadarMesh->GetStaticMesh()->GetBounds().GetBox().GetSize().ToString()));
}

