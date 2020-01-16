// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LockOnSystem.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class ULockOnSystem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SURVIVE_API ILockOnSystem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "LockOn")
		void LockOn(bool& Success);
};
