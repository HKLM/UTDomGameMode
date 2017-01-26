// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTPlayerController.h"
#include "UTDomGameState.h"
//#include "UTDomPlayerState.h"
#include "UTDomPlayerController.h"

AUTDomPlayerController::AUTDomPlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AUTDomPlayerController::ServerSwitchTeam_Implementation()
{
	AUTDomGameState* GS = GetWorld()->GetGameState<AUTDomGameState>();
	if (GS == NULL)
	{
		return;
	}
	uint8 NumOfTeams = GS->Teams.Num(); -1;
	if (UTPlayerState && UTPlayerState->Team && (UTPlayerState->Team->TeamIndex != 255))
	{
		uint8 i = UTPlayerState->Team->TeamIndex;
		if (UTPlayerState->bPendingTeamSwitch)
		{
			UTPlayerState->bPendingTeamSwitch = false;
		}
		else if (!GetWorld()->GetAuthGameMode()->HasMatchStarted())
		{
			if (UTPlayerState->bIsWarmingUp)
			{
				// no team swaps while warming up
				return;
			}
			else
			{
				if (i + 1 <= NumOfTeams && GS->Teams.IsValidIndex(i + 1))
				{
					// Go to next higher team index
					ChangeTeam(i + 1);
				}
				else if (i + 1 > NumOfTeams)
				{
					//Go to the 1st team
					ChangeTeam(0);
				}
			}
			if (UTPlayerState->bPendingTeamSwitch)
			{
				UTPlayerState->SetReadyToPlay(false);
			}
		}
		else
		{
			if (i + 1 <= NumOfTeams && GS->Teams.IsValidIndex(i + 1))
			{
				ChangeTeam(i + 1);
			}
			else if (i + 1 > NumOfTeams)
			{
				ChangeTeam(0);
			}
		}
		UTPlayerState->ForceNetUpdate();
	}
}

