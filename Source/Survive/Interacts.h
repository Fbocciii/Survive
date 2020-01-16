// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interacts.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType) //Previously 'MinimalAPI' specifier
class UInteracts : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SURVIVE_API IInteracts
{
	GENERATED_BODY()
		
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void Interact(const AActor* Interactor, bool& Success);
};
