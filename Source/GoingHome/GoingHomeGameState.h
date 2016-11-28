// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameStateBase.h"
#include "GoingHomeGameState.generated.h"

USTRUCT()
struct FDialogEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName EventId;

	UPROPERTY(EditAnywhere)
	int32 Time;

	UPROPERTY(EditAnywhere)
	FText Text;

	UPROPERTY(EditAnywhere)
	TArray<FName> PlayerActions;

	UPROPERTY(EditAnywhere)
	TArray<FName> PlayerActionArguments;

	UPROPERTY(EditAnywhere)
	FName AfterEvent;
};

/**
 * 
 */
UCLASS()
class GOINGHOME_API AGoingHomeGameState : public AGameStateBase
{
	GENERATED_BODY()

	FTimerHandle EventTimerHandle;

	UPROPERTY(EditAnywhere)
	TArray<FDialogEvent> Events;

	UPROPERTY(EditAnywhere)
	int32 CurrentEventIndex;

	bool timerSet;

	void TimerHandler();

public:
	AGoingHomeGameState();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString DisplayText;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnPitch(float value);

	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnYaw(float value);

	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnRoll(float value);

	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnThrust(float value);

	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnOverlap(FName other);

	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnMine(FName other);

	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnMineDestroyed(FName other);

	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnProjectileHit(AActor const* shooter, AActor const* victim);

private:
	void HandleInteractionEvent(FName type, FName other);
	void HandleShipEvent(FName type);
	void HandleShipEvent(int index);
	
};
