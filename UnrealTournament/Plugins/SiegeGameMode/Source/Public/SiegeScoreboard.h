// Created by Brian 'Snake' Alexander, (c) 2016
#pragma once 

#include "UnrealTournament.h"
#include "UTTeamScoreboard.h"
#include "SiegeScoreboard.generated.h"

UCLASS()
class USiegeScoreboard : public UUTTeamScoreboard
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void DrawPlayerStats(AUTPlayerState* PS, float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo) override;

	virtual void DrawTeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo) override;
};
