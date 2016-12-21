// Fill out your copyright notice in the Description page of Project Settings.

#include "GoingHome.h"
#include "PawnBase.h"


// Sets default values
APawnBase::APawnBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));

	if (ShipMesh != nullptr)
	{
		ShipMesh->AlwaysLoadOnClient = true;
		ShipMesh->AlwaysLoadOnServer = true;
		ShipMesh->bOwnerNoSee = false;
		ShipMesh->bCastDynamicShadow = true;
		ShipMesh->bAffectDynamicIndirectLighting = true;
		
		SetRootComponent(ShipMesh);
	}
}


