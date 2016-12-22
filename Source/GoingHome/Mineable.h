// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Mineable.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UMineable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class GOINGHOME_API IMineable
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dummy")
	bool Dummy();
};
