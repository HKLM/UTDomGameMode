// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameMode.h"
#include "ControlPoint.h"
#include "UTDomControlPoint.h"
#include "UTADomTypes.h"
#include "MultiTeamTeamInfo.h"
#include "Domination.h"

ADomination::ADomination(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = NSLOCTEXT("UTDomGameMode", "DOM", "Domination");
	GoalScore = 100;
	MinPlayersToStart = 1;
}

void ADomination::DefaultTimer()
{
	Super::DefaultTimer();
	if (HasMatchStarted() && !HasMatchEnded())
	{
		float c = 0.2f;
		if (TimeLimit > 0)
		{
			// Award more points toward the end of the match
			if (DomGameState->GetRemainingTime() < 0.25 * TimeLimit)
			{
				c = (DomGameState->GetRemainingTime() < 0.1 * TimeLimit) ? 0.8f : 0.4f;
			}
		}

		for (uint8 cp = 0; cp < CDomPoints.Num(); cp++)
		{
			ScoreTeam(cp, c);
		}
	}
}

void ADomination::ScoreTeam(uint8 ControlPointIndex, float TeamScoreAmount)
{
	if ((CDomPoints[ControlPointIndex] != nullptr && CDomPoints[ControlPointIndex]->ControllingTeam != nullptr) && CDomPoints[ControlPointIndex]->GetIsScoreReady())
	{
		for (uint8 i = 0; i < NumTeams; i++)
		{
			if (CDomPoints[ControlPointIndex]->ControllingTeam->GetTeamNum() == i)
			{
				if (CDomPoints[ControlPointIndex]->ControllingPawn != nullptr)
				{
					// award points to player
					CDomPoints[ControlPointIndex]->ControllingPawn->Score += TeamScoreAmount;
					CDomPoints[ControlPointIndex]->UpdateHeldPointStat(CDomPoints[ControlPointIndex]->ControllingPawn, TeamScoreAmount);
				}
				// award points to players team
				CDomPoints[ControlPointIndex]->ControllingTeam->SetFloatScore(TeamScoreAmount);
				CDomPoints[ControlPointIndex]->ControllingTeam->ForceNetUpdate();
				CheckScore(CDomPoints[ControlPointIndex]->ControllingPawn);
			}
		}
	}
}
