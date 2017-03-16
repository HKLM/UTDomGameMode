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

	// Flag this player as not being idle
	if (UTPlayerState) UTPlayerState->NotIdle();

	if (UTPlayerState != nullptr && UTPlayerState->Team != nullptr)
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
				ChangeTeam(NewTeam);
				if (PS->bIsWarmingUp)
				{
					ServerToggleWarmup();
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
