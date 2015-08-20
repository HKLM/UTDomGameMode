// Created by Brian 'Snake' Alexander, 2015

#pragma once

#include "UnrealTournament.h"
#include "UTATypes.h"
#include "UTADomTypes.generated.h"

/**
 * Values that represent dom objective types.
 */
UENUM()
enum class EDomObjectiveType : uint8
{
	ControlPoint,
	CampingPoint,
	xPoint,
	Disabled
};

/**
 * Adds Dom Team events to CarriedObjectState.
 * @note	Currently does not do anything
 */
namespace CarriedObjectState
{
	const FName Red = FName(TEXT("Red"));
	const FName Blue = FName(TEXT("Blue"));
	const FName Green = FName(TEXT("Green"));
	const FName Gold = FName(TEXT("Gold"));
	const FName Neutral = FName(TEXT("Neutral"));
}
