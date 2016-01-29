// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameMode.h"
#include "ControlPoint.h"
#include "UTDomTeamInfo.h"
#include "Domination.h"

ADomination::ADomination(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = NSLOCTEXT("UTDomGameMode", "DOM", "Domination");
	GoalScore = 100;
	MinPlayersToStart = 1;
	MaxControlPoints = 8;
}

void ADomination::DefaultTimer()
{
	Super::DefaultTimer();
	if (HasMatchStarted() && !HasMatchEnded())
	{
		float c = 0.2;
		if (TimeLimit > 0)
		{
			// Award more points toward the end of the match
			if (DomGameState->RemainingTime < 0.25 * TimeLimit)
			{
				if (DomGameState->RemainingTime < 0.1 * TimeLimit){
					c = 0.8;
				}
				else{
					c = 0.4;
				}
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
	if ((CDomPoints[ControlPointIndex] != NULL && CDomPoints[ControlPointIndex]->ControllingTeam != NULL) && CDomPoints[ControlPointIndex]->bScoreReady)
	{
		for (uint8 i = 0; i < NumTeams; i++)
		{
			if (CDomPoints[ControlPointIndex]->ControllingTeam->GetTeamNum() == i)
			{
				if (CDomPoints[ControlPointIndex]->ControllingPawn != NULL)
				{
					CDomPoints[ControlPointIndex]->ControllingPawn->Score += TeamScoreAmount;
					CDomPoints[ControlPointIndex]->UpdateHeldPointStat(CDomPoints[ControlPointIndex]->ControllingPawn, TeamScoreAmount);
				}
				CDomPoints[ControlPointIndex]->ControllingTeam->SetFloatScore(TeamScoreAmount);
				CDomPoints[ControlPointIndex]->ControllingTeam->ForceNetUpdate();
				CheckScore(CDomPoints[ControlPointIndex]->ControllingPawn);
			}
		}
	}
}
