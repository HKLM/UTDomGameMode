// Created by Brian 'Snake' Alexander, 2015
#pragma once 

#include "UnrealTournament.h"
#include "UTTeamScoreboard.h"
#include "UTDomScoreboard.generated.h"

UCLASS()
class UUTDomScoreboard : public UUTTeamScoreboard
{
	GENERATED_UCLASS_BODY()

protected:

	virtual void DrawGamePanel(float RenderDelta, float& YOffset) override;
	virtual void DrawGameOptions(float RenderDelta, float& YOffset) override;
};
