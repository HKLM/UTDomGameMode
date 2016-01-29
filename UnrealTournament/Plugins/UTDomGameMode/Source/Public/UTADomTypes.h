// Created by Brian 'Snake' Alexander, 2015

#pragma once

#include "UnrealTournament.h"
#include "UTADomTypes.generated.h"

/**
 * Values that represent ControlPoint objective current state
 */
UENUM(BlueprintType)
namespace EControllingTeam
{
	enum Type
	{
		TE_Red		UMETA(DisplayName = "Red"),
		TE_Blue		UMETA(DisplayName = "Blue"),
		TE_Green	UMETA(DisplayName = "Green"),
		TE_Gold		UMETA(DisplayName = "Gold"),
		TE_Neutral	UMETA(DisplayName = "Neutral"),
		TE_Disabled	UMETA(DisplayName = "Disabled")
	};
}
