// Created by Brian 'Snake' Alexander, 2015
#pragma once 

#include "UnrealTournament.h"
#include "UTTeamScoreboard.h"
#include "UTDomGameState.h"
#include "UTDomScoreboard.generated.h"

UCLASS(NotPlaceable)
class UUTDomScoreboard : public UUTTeamScoreboard
{
	GENERATED_UCLASS_BODY()
		virtual void Draw_Implementation(float DeltaTime) override;
	virtual void DrawPlayerStats(AUTPlayerState* PS, float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo) override;

protected:
	virtual void DrawScorePanel(float RenderDelta, float& YOffset) override;
	virtual void DrawMultiScorePanel(float RenderDelta, float& YOffset);
	virtual void DrawGamePanel(float RenderDelta, float& YOffset) override;
	virtual void DrawGameOptions(float RenderDelta, float& YOffset) override;
	virtual void DrawTeamPanel(float RenderDelta, float& YOffset) override;
	virtual void DrawMultiTeamPanel(float RenderDelta, float& YOffset);
	virtual void DrawPlayerScores(float RenderDelta, float& DrawY) override;
	virtual void DrawMultiPlayerScores(float RenderDelta, float& DrawY);
};
