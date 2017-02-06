// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTDomGameMode.h"
#include "UTDomGameState.h"
#include "UTDomGameMessage.h"
#include "UTDomPlayerController.h"
#include "UTDomCharacter.h"
#include "UTPlayerState.h"
#include "UTDomPlayerState.h"

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
		AUTDomPlayerController* PC = Cast<AUTDomPlayerController>(Controller);
		if (PC)
		{
			PC->ClientReceiveLocalizedMessage(UUTDomGameMessage::StaticClass(), Switch, this, NULL, NULL);
		}
	}
}

void AUTDomPlayerState::ServerRequestChangeTeam_Implementation(uint8 NewTeamIndex)
{
	AUTDomGameMode* Game = GetWorld()->GetAuthGameMode<AUTDomGameMode>();
	if (Game != NULL && Game->bTeamGame)
	{
		AController* Controller = Cast<AController>(GetOwner());
		if (Controller != NULL)
		{
			AUTDomPlayerController* DomCon = Cast<AUTDomPlayerController>(Controller);
			if (DomCon)
			{
				if (!GetWorld()->GetGameState<AUTDomGameState>()->Teams.IsValidIndex(NewTeamIndex) || NewTeamIndex == 255)
				{
					NewTeamIndex = 0;
				}
				if (Game->ChangeTeam(DomCon, NewTeamIndex, true))
				{
					HandleTeamChanged(DomCon);
					MakeTeamSkin(NewTeamIndex);
				}
			}
		}
	}
}

void AUTDomPlayerState::MakeTeamSkin(uint8 NewTeamIndex)
{
	AUTDomCharacter* UTC = Cast<AUTDomCharacter>(GetUTCharacter());
	AUTDomGameState* DomGS = Cast<AUTDomGameState>(GetWorld()->GetGameState<AUTGameState>());
	if (DomGS && UTC && DomGS->Teams.IsValidIndex(NewTeamIndex))
	{
		/* value what base color to use (red or blue) */
		UTC->TeamBodySkinColor = DomGS->TeamBodySkinColor[NewTeamIndex];
		UTC->TeamSkinOverlayColor = DomGS->TeamSkinOverlayColor[NewTeamIndex];
		UTC->SetTeamSkin(NewTeamIndex);
	}
}
