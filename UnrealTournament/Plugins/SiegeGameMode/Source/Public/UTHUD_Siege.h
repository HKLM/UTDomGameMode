// Created by Brian 'Snake' Alexander, (c) 2016
#pragma once

#include "UnrealTournament.h"
#include "UTHUD.h"
#include "UTHUD_Siege.generated.h"

UCLASS()
class AUTHUD_Siege : public AUTHUD
{
	GENERATED_UCLASS_BODY()

	virtual FLinearColor GetBaseHUDColor() override;
	FLinearColor GetWidgetTeamColor();
};
