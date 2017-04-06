// Created by Brian 'Snake' Alexander, 2015

#pragma once

#include "UnrealTournament.h"
#include "UTADomTypes.generated.h"

/**
 * Values of what type of Control Point this is. 
 * Control Point for DOM or in DDOM the xDomPoint is 'A' or 'B'
 */
UENUM(BlueprintType)
namespace EControlPoint
{
	enum Type
	{
		PT_ControlPoint		UMETA(DisplayName = "Control Point"),
		PT_xDomA			UMETA(DisplayName = "Point A"),
		PT_xDomB			UMETA(DisplayName = "Point B"),
		PT_Disabled			UMETA(DisplayName = "Disabled")
	};
}