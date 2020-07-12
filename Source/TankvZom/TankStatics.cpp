// Fill out your copyright notice in the Description page of Project Settings.


#include "TankStatics.h"

float UTankStatics::FindDeltaAngleDegrees(float A1, float A2)
{
	// Find the difference
	float Delta = A2 - A1;

	if (Delta > 180.0f)
	{
		Delta = Delta - 360.0f;
	}
	else if (Delta < -180.0f)
	{
		Delta = Delta + 360.0f;
	}

	return Delta;
}
