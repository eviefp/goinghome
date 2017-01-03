// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "RadarComponent.generated.h"

/**
 * 
 */
UCLASS()
class GOINGHOME_API URadarComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
	
public:
	static FName RadarMaterialName;


	URadarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar")
	class USphereComponent* DetectionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar")
	class UStaticMesh* RadarEntityMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar")
	class UWidgetComponent* UIComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar")
	TSubclassOf<class UUserWidget> RadarWidgetClass;

private:
	TMap<AActor*, class UStaticMeshComponent*> OverlappingActors;

	

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
