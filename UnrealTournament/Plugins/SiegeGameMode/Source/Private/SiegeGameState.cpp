// Copyright 2016 Brian 'Snake' Alexander, All Rights Reserved.

#include "UnrealTournament.h"
#include "SiegePoint.h"
#include "Net/UnrealNetwork.h"
#include "UTGameState.h"
#include "UTDomStat.h"
#include "SiegeGameState.h"

ASiegeGameState::ASiegeGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameScoreStats.Add(NAME_RegularKillPoints);
	GameScoreStats.Add(NAME_ControlPointCaps);
	GameScoreStats.Add(NAME_ControlPointHeldTime);
	GameScoreStats.Add(NAME_ControlPointHeldPoints);

	TeamStats.Add(NAME_TeamControlPointHeldTime);
}

void ASiegeGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASiegeGameState, GameSiegePoints);
}

void ASiegeGameState::RegisterSiegePoint(ASiegePoint* DomObj, bool bIsDisabled)
{
	GameSiegePoints.AddUnique(DomObj);
}

void ASiegeGameState::UpdateSiegePointFX(ASiegePoint* ThePoint, uint8 NewTeamIndex)
{
	if (NewTeamIndex == 255)
	{
		NewTeamIndex = 4;
	}
	for (uint8 i = 0; i < GameSiegePoints.Num(); i++)
	{
		if (GameSiegePoints[i] == ThePoint)
		{
			GameSiegePoints[i]->UpdateTeamEffects(NewTeamIndex);
			return;
		}
	}
}

AUTTeamInfo* ASiegeGameState::FindLeadingTeam()
{
	AUTTeamInfo* WinningTeam = NULL;
	bool bTied;

	if (Teams.Num() > 0)
	{
		WinningTeam = Teams[0];
		bTied = false;
		for (uint8 i = 1; i < Teams.Num(); i++)
		{
			if (Teams[i]->Score == WinningTeam->Score)
			{
				bTied = true;
			}
			else if (Teams[i]->Score > WinningTeam->Score)
			{
				WinningTeam = Teams[i];
				bTied = false;
			}
		}

		if (bTied)
		{
			WinningTeam = NULL;
		}
	}
	return WinningTeam;
}

void ASiegeGameState::SetWinner(AUTPlayerState* NewWinner)
{
	WinnerPlayerState = NewWinner;
	WinningTeam = FindLeadingTeam();
	ForceNetUpdate();
}

AUTPlayerState* ASiegeGameState::FindBestPlayerOnTeam(uint8 TeamNumToTest)
{
	AUTPlayerState* Best = NULL;
	for (uint8 i = 0; i < PlayerArray.Num(); i++)
	{
		AUTPlayerState* PS = Cast<AUTPlayerState>(PlayerArray[i]);
		if (PS != NULL && PS->GetTeamNum() == TeamNumToTest && (Best == NULL || Best->Score < PS->Score))
		{
			Best = PS;
		}
	}
	return Best;
}

int32 ASiegeGameState::GetOtherTeamScore(uint8 WinningTeamIndex) const
{
	if (WinningTeamIndex == NULL) WinningTeamIndex = 0;

	int32 outputScore = 0;
	for (uint8 i = 0; i < Teams.Num(); i++)
	{
		// Not 4 team compatible
		if (Teams[i]->TeamIndex != WinningTeamIndex)
		{
			outputScore = Teams[i]->Score;
			break;
		}
	}
	return outputScore;
}

FText ASiegeGameState::GetGameStatusText(bool bForScoreboard)
{
	if (bForScoreboard)
	{
		return (GoalScore == 0) ? FText::Format(NSLOCTEXT("SiegeGameState", "GoalTimeFormat", "Score most points in {0} minutes"), FText::AsNumber(TimeLimit)) : FText::Format(NSLOCTEXT("SiegeGameState", "GoalScoreFormat", "First Team to {0} Points"), FText::AsNumber(GoalScore));
	}

	return Super::GetGameStatusText(bForScoreboard);
}
