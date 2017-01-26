// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTGameMode.h"
#include "UTDomGameMessage.h"
//#include "UTDomCharacter.h"
#include "UTDomPlayerController.h"
//#include "UTDomPlayerState.h"

AUTDomPlayerState::AUTDomPlayerState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AUTDomPlayerState::HandleTeamChanged(AController* Controller)
{
	AUTCharacter* Pawn = GetUTCharacter();
	if (Pawn != NULL)
	{
		Pawn->PlayerChangedTeam();
	}
	if (Team)
	{
		int32 Switch = Team->TeamIndex + 9;
		AUTPlayerController* PC = Cast<AUTPlayerController>(Controller);
		if (PC)
		{
			PC->ClientReceiveLocalizedMessage(UUTDomGameMessage::StaticClass(), Switch, this, NULL, NULL);
		}
	}
}

void AUTDomPlayerState::ServerRequestChangeTeam_Implementation(uint8 NewTeamIndex)
{
	AUTGameMode* Game = GetWorld()->GetAuthGameMode<AUTGameMode>();
	if (Game != NULL && Game->bTeamGame)
	{
		AUTDomPlayerController* Controller = Cast<AUTDomPlayerController>(GetOwner());
		if (Controller != NULL)
		{
			if (NewTeamIndex == 255 && Team != NULL)
			{
				NewTeamIndex = (Team->TeamIndex + 1) % FMath::Max<uint8>(1, GetWorld()->GetGameState<AUTGameState>()->Teams.Num());
			}
			else if (!GetWorld()->GetGameState<AUTGameState>()->Teams.IsValidIndex(NewTeamIndex))
			{
				NewTeamIndex = 0;
			}
		}

		if (Game->ChangeTeam(Controller, NewTeamIndex, true))
		{
			HandleTeamChanged(Controller);
		}
	}
}

