#pragma once

#include "UnrealTournament.h"
#include "UTBasePlayerController.h"
#include "UTGameState.h"
#include "UTPlayerController.h"
#include "UTDomPlayerController.generated.h"

UCLASS()
class AUTDomPlayerController : public AUTPlayerController
{
	GENERATED_UCLASS_BODY()

		virtual void ServerSwitchTeam_Implementation() override;
};
