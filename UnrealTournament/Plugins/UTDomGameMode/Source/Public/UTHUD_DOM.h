// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTHUD.h"
#include "UTHUD_DOM.generated.h"

UCLASS()
class UTDOMGAMEMODE_API AUTHUD_DOM : public AUTHUD
{
	GENERATED_UCLASS_BODY()

	virtual FLinearColor GetBaseHUDColor() override;
	FLinearColor GetWidgetTeamColor();
};
