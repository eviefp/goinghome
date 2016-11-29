// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"


#ifdef UE_BUILD_DEBUG
#define GH_LOG_ERROR(x) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, x)
#else
#define GH_LOG_ERROR(x) (x)
#endif

DECLARE_LOG_CATEGORY_EXTERN(GoingHomeLog, Log, All);
