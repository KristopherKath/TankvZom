// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TankStatics.generated.h"

/**
 * 
 */
UCLASS()
class TANKVZOM_API UTankStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


	/** Calculate angle difference in the range [-180, 180]. */
	UFUNCTION(BlueprintCallable, Category = "Math")
	static float FindDeltaAngleDegrees(float A1, float A2);

	/* Finds the angle between two 2D points. */
};
