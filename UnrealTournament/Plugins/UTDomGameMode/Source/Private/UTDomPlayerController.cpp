// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTDomPlayerState.h"
#include "UTPlayerController.h"
#include "UTDomPlayerController.h"

AUTDomPlayerController::AUTDomPlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AUTDomPlayerController::ServerSwitchTeam_Implementation()
{
	// Ranked sessions don't allow team changes
	AUTGameMode* GameMode = GetWorld()->GetAuthGameMode<AUTGameMode>();
	if (GameMode && GameMode->bRankedSession)
	{
		return;
	}
	if (UTPlayerState != NULL && UTPlayerState->Team != NULL)
	{
		AUTDomPlayerState* PS = Cast<AUTDomPlayerState>(UTPlayerState);
		if (PS)
		{
			uint8 testteam = PS->Team->TeamIndex + 1;
			uint8 NewTeam = (GetWorld()->GetGameState<AUTDomGameState>()->Teams.IsValidIndex(testteam)) ? testteam : 0;
			if (PS->bPendingTeamSwitch)
			{
				PS->bPendingTeamSwitch = false;
			}
			else if (!GetWorld()->GetAuthGameMode()->HasMatchStarted())
			{
				if (PS->bIsWarmingUp)
				{
					// no team swaps while warming up
					return;
				}
				ChangeTeam(NewTeam);
				if (PS->bPendingTeamSwitch)
				{
					PS->SetReadyToPlay(false);
				}
			}
			else
			{
				ChangeTeam(NewTeam);
			}
			PS->ForceNetUpdate();
		}
	}
}
