// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UProjectile : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class GOINGHOME_API IProjectile
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyCategory")
	bool SetProjectileParent(AActor* parent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyCategory")
	AActor* GetProjectileParent() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyCategory")
	void SetLinearVelocity(const FVector& velocity);
	
};
