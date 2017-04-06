// Created by Brian 'Snake' Alexander, 2017
// 4 Team Scoreboard
#pragma once 

#include "UnrealTournament.h"
#include "MultiTeamScoreboard.h"
#include "UTDomScoreboard.generated.h"

UCLASS()
class UTDOMGAMEMODE_API UUTDomScoreboard : public UMultiTeamScoreboard
{
	GENERATED_UCLASS_BODY()

protected:

	virtual void DrawTeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo);
	virtual void Draw4TeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo);
};
