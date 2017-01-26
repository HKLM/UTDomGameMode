/**
* Adds 4 team support
* Created by Brian 'Snake' Alexander, 2016
**/
#pragma once

#include "UnrealTournament.h"
#include "UTPlayerController.h"
#include "UTDomPlayerController.generated.h"

UCLASS()
class UTDOMGAMEMODE_API AUTDomPlayerController : public AUTPlayerController
{
	GENERATED_UCLASS_BODY()

	virtual void ServerSwitchTeam_Implementation() override;
};
