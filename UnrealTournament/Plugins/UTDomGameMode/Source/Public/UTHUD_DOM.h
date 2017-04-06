// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTHUD_MultiTeam.h"
#include "UTHUD_DOM.generated.h"

UCLASS()
class UTDOMGAMEMODE_API AUTHUD_DOM : public AUTHUD_MultiTeam
{
	GENERATED_UCLASS_BODY()

	virtual void DrawMinimapSpectatorIcons() override;
};
