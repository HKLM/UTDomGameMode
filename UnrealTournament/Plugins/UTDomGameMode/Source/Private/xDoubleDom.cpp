// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTDoubleDomGameMode.h"
#include "UTDomTeamInfo.h"
#include "xDoubleDom.h"

AxDoubleDom::AxDoubleDom(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = NSLOCTEXT("UTDomGameMode", "DDOM", "Double Domination");
	GoalScore = 10;
	MinPlayersToStart = 1;
	bAllowTranslocator = false;
}
