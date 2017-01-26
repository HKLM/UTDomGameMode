// Created by Brian 'Snake' Alexander, 2015

#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "Net/UnrealNetwork.h"
#include "UTGameState.h"
#include "UTDomPlayerState.h"
#include "UTDomStat.h"
#include "UTDomGameState.h"

AUTDomGameState::AUTDomGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameScoreStats.Add(NAME_RegularKillPoints);
	GameScoreStats.Add(NAME_ControlPointCaps);
	GameScoreStats.Add(NAME_ControlPointHeldTime);
	GameScoreStats.Add(NAME_ControlPointHeldPoints);

	TeamStats.Add(NAME_TeamControlPointHeldTime);
}

void AUTDomGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUTDomGameState, GameControlPoints);
}

void AUTDomGameState::RegisterControlPoint(AControlPoint* DomObj, bool bIsDisabled)
{
	if (DomObj)
	{
		if (!bIsDisabled)
		{
			GameControlPoints.AddUnique(DomObj);
		}
		else
		{
			DomObj->DisablePoint();
		}
	}
}

void AUTDomGameState::UpdateControlPointFX(AControlPoint* ThePoint, uint8 NewTeamIndex)
{
	if (NewTeamIndex == 255)
	{
		NewTeamIndex = 4;
	}
	for (uint8 i = 0; i < GameControlPoints.Num(); i++)
	{
		if (GameControlPoints[i] == ThePoint)
		{
			GameControlPoints[i]->UpdateTeamEffects(NewTeamIndex);
			return;
		}
	}
}

AUTDomTeamInfo* AUTDomGameState::FindLeadingTeam()
{
	AUTDomTeamInfo* TheWinningTeam = Cast<AUTDomTeamInfo>(Teams[0]);
	bool bNoScore = true;
	if (Teams.Num() > 0)
	{
		for (uint8 i = 1; i < Teams.Num(); i++)
		{
			if (Cast<AUTDomTeamInfo>(Teams[i])->GetFloatScore() > TheWinningTeam->GetFloatScore())
			{
				bNoScore = false;
				TheWinningTeam = Cast<AUTDomTeamInfo>(Teams[i]);
			}
		}
	}
	if (bNoScore)
	{
		return NULL;
	}
	else
	{
		return TheWinningTeam;
	}
}

void AUTDomGameState::SetWinner(AUTPlayerState* NewWinner)
{
	WinnerPlayerState = NewWinner;
	WinningTeam = FindLeadingTeam();
	ForceNetUpdate();
}

AUTPlayerState* AUTDomGameState::FindBestPlayerOnTeam(uint8 TeamNumToTest)
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

FText AUTDomGameState::GetGameStatusText(bool bForScoreboard)
{
	if (bForScoreboard)
	{
		return (GoalScore == 0) 
			? FText::Format(NSLOCTEXT("UTDomGameState", "GoalTimeFormat", "Score most points in {0} minutes"), FText::AsNumber(TimeLimit)) 
			: FText::Format(NSLOCTEXT("UTDomGameState", "GoalScoreFormat", "First Team to {0} Points"), FText::AsNumber(GoalScore));
	}

	return Super::GetGameStatusText(bForScoreboard);
}
