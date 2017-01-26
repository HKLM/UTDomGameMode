/**
* Adds 4 team support
* Created by Brian 'Snake' Alexander, 2016
**/
#pragma once

#include "UnrealTournament.h"
#include "UTPlayerState.h"
#include "UTDomPlayerState.generated.h"

UCLASS()
class UTDOMGAMEMODE_API AUTDomPlayerState : public AUTPlayerState
{
	GENERATED_UCLASS_BODY()

	virtual void HandleTeamChanged(AController* Controller) override;
	virtual void ServerRequestChangeTeam_Implementation(uint8 NewTeamIndex) override;
};
