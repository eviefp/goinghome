// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameStateBase.h"
#include "GoingHomeGameState.generated.h"

/**
* Struct that holds a single event.
*/
USTRUCT()
struct FDialogEvent
{
	GENERATED_BODY()

	/** EventId should probably be unique. It will be used if/when we implement AfterEvent. */
	UPROPERTY(EditAnywhere)
	FName EventId;

	/** Time the event should be displayed on screen. If it has an action, then it's the time it will keep being displayed after action is completed. */
	UPROPERTY(EditAnywhere)
	int32 Time;

	/** Text to show the player for this event. */
	UPROPERTY(EditAnywhere)
	FText Text;

	/** Array of string actions. These are connected to pawn events for now. */
	UPROPERTY(EditAnywhere)
	TArray<FName> PlayerActions;

	/** Arguments for player actions. This should really  be refactored into PlayerActions (TArray<SomeStruct>). */
	UPROPERTY(EditAnywhere)
	TArray<FName> PlayerActionArguments;

	/** Currently not used. The idea was that this event should only be active after this event is complete - so you could create branches. */
	UPROPERTY(EditAnywhere)
	FName AfterEvent;
};


/**
 * Main game state, handles dialog/events for now.
 */
UCLASS()
class GOINGHOME_API AGoingHomeGameState : public AGameStateBase
{
	GENERATED_BODY()


public:
	//~=============================================================================
	// Constructors and overrides.

	/** 
	 * Basic constructor for initialization. Should probably try to do more here, 
	 * as long as it doens't require interacting with other actors. 
	 */
	AGoingHomeGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Initialisation that depends on other actors being alive or available. */
	virtual void BeginPlay() override;


	//~=============================================================================
	// Ship Events
	
	/** 
	 * Mined amount incremented everytime we mine an asteroid. 
	 * Used by BP_HUD_Debug.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mining")
	int32 MinedAmount;


	//~=============================================================================
	// Dialog Events

	/** 
	 * Current event display text. If empty, it will not show anything (not even 
	 * the bar below; set to " " to show it with no text). 
	 * Used by BP_HUD_Dialog.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DialogEvents")
	FString DisplayText;

	/** Array of all level events. Visible for debugging purposes. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DialogEvents")
	TArray<struct FDialogEvent> Events;

	/** 
	 * Current event as an index in the Events array. Visible for debugging 
	 * porposes only. 
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DialogEvents")
	int32 CurrentEventIndex;


	//~=============================================================================
	// Ship Events

	/** Handler for pitch input axis. Triggered by BP_SHIP. */
	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnPitch(float value);

	/** Handler for yaw input axis. Triggered by BP_SHIP. */
	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnYaw(float value);

	/** Handler for roll input axis. Triggered by BP_SHIP. */
	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnRoll(float value);

	/** Handler for thrust input axis. Triggered by BP_SHIP. */
	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnThrust(float value);

	/** Handler for the overlap event. Triggered by BP_SHIP. */
	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnOverlap(FName other);

	/** Handler for the mining event. Triggered by BP_SHIP. */
	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnMine(FName other);

	/** Handler for the mining destroyed. Triggered by BP_SHIP. */
	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnMineDestroyed(FName other);

	/** Handler for the projectile hit event. Triggered by BP_ShipProjectile. */
	UFUNCTION(BlueprintCallable, Category = "ShipEvents")
	void OnProjectileHit(class AActor* shooter, class AActor* victim);


private:
	//~=============================================================================
	// Helpers

	/** Initialisation - called from constructor and BeginPlay. */
	void Initialise();

	/** Timer handle for the dialog events timers. */
	struct FTimerHandle EventTimerHandle;

	/** True if a timer has already been set, to avoid resetting it on events. */
	bool timerSet;

	/** 
	 * Callback for the timer. Increments CurrentEventIndex, resets timerSet and  
	 * sets up next timer if needed.
	 * TODO: fix the indexing issue where you need an extra dummy event at the end.
	 */
	void TimerHandler();

	/** 
	 * Helper used for event actions that take a parameter. Calls HandleShipEvent(int)
	 * because there might be multiple identical events with different parameters.
	 */
	void HandleInteractionEvent(FName type, FName other);

	/** 
	 * Finds the index of first action in CurrentEventIndex / PlayerActions that 
	 * matches type and calls HandleShipEvent(int).
	 */
	void HandleShipEvent(FName type);

	/** 
	 * Removes the index action from CurrentEventIndex / PlayerActions. If there are 
	 * no actions left, queues the timer. 
	 */
	void HandleShipEvent(int index);
	
};
