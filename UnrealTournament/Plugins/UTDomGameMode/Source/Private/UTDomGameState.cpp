// Copyright 2007-2015 Brian 'Snake' Alexander, All Rights Reserved.

#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "Net/UnrealNetwork.h"
#include "UTDomGameState.h"

DEFINE_LOG_CATEGORY(UTDomGameState);

AUTDomGameState::AUTDomGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AUTDomGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//TODO - Change to only replicate TeamNum not entire point
	DOREPLIFETIME(AUTDomGameState, GameControlPoints);
	DOREPLIFETIME_CONDITION(AUTDomGameState, KingOfTheHill, COND_InitialOnly);
}

void AUTDomGameState::RegisterControlPoint(AControlPoint* DomObj, bool bIsDisabled)
{
	if (DomObj != NULL)
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

void AUTDomGameState::UpdateControlPointFX(AControlPoint* ThePoint, int32 NewTeamIndex)
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

AUTTeamInfo* AUTDomGameState::FindLeadingTeam()
{
	AUTTeamInfo* WinningTeam = NULL;
	bool bTied;

	if (Teams.Num() > 0)
	{
		WinningTeam = Teams[0];
		bTied = false;
		for (int i = 1; i < Teams.Num(); i++)
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

void AUTDomGameState::SetWinner(AUTPlayerState* NewWinner)
{
	WinnerPlayerState = NewWinner;
	WinningTeam = FindLeadingTeam();
	ForceNetUpdate();
}

AUTPlayerState* AUTDomGameState::FindBestPlayerOnTeam(int32 TeamNumToTest)
{
	AUTPlayerState* Best = NULL;
	for (int32 i = 0; i < PlayerArray.Num(); i++)
	{
		AUTPlayerState* PS = Cast<AUTPlayerState>(PlayerArray[i]);
		if (PS != NULL && PS->GetTeamNum() == TeamNumToTest && (Best == NULL || Best->Score < PS->Score))
		{
			Best = PS;
		}
	}
	return Best;
}
