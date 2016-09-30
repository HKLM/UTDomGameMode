// Created by Brian 'Snake' Alexander, 2015
#pragma once 

#include "UnrealTournament.h"
#include "UTTeamScoreboard.h"
#include "UTDomScoreboard.generated.h"

UCLASS()
class UTDOMGAMEMODE_API UUTDomScoreboard : public UUTTeamScoreboard
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void DrawTeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo) override;
};
