// Created by Brian 'Snake' Alexander, 2017
// 4 Team Scoreboard
#pragma once 

#include "UnrealTournament.h"
#include "UTHUDWidget.h"
#include "UTScoreboard.h"
#include "UTDomScoreboard.generated.h"

UCLASS()
class UTDOMGAMEMODE_API UUTDomScoreboard : public UUTScoreboard
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard")
		FText RedTeamText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard")
		FText BlueTeamText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard")
		FText GreenTeamText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard")
		FText GoldTeamText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard")
		bool bUseRoundKills;

	virtual void Draw_Implementation(float DeltaTime) override;
	virtual void SelectNext(int32 Offset, bool bDoNoWrap = false);
	virtual void SelectionLeft();
	virtual void SelectionRight();

protected:
	/** Cache NumTeams value from GameState. Used to determin if green and/or gold teams are drawn */
	UPROPERTY()
		int8 GameNumTeams;

	UPROPERTY()
		FText TeamScoringHeader;

	virtual void Draw4ScorePanel(float RenderDelta, float& YOffset);

	virtual void DrawTeamPanel(float RenderDelta, float& YOffset) override;
	virtual void Draw4TeamPanel(float RenderDelta, float& YOffset);

	virtual void Draw4ScoreHeaders(float RenderDelta, float& DrawY);

	virtual void DrawPlayerScores(float RenderDelta, float& DrawY) override;
	virtual void Draw4PlayerScores(float RenderDelta, float& DrawY);

	/** 5coring breakdown for Teams. */
	virtual void DrawTeamScoreBreakdown(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom);
	virtual void Draw4TeamScoreBreakdown(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom);

	virtual void DrawTeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo);
	virtual void Draw4TeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo);

	/** Draw one line of scoring breakdown where values are clock stats. */
	virtual void DrawClockTeamStatsLine(FText StatsName, FName StatsID, float DeltaTime, float XOffset, float& YPos, const FStatsFontInfo& StatsFontInfo, float ScoreWidth, bool bSkipEmpty);

	/** Return player with most kills for Team. */
	virtual AUTPlayerState* FindTopTeamKillerFor(uint8 TeamNum);

	/** Return player with best kill/death ratio for Team. */
	virtual AUTPlayerState* FindTopTeamKDFor(uint8 TeamNum);

	/** Return player with best score per minute for Team. */
	virtual AUTPlayerState* FindTopTeamSPMFor(uint8 TeamNum);

	/** Return player with best score for Team. */
	virtual AUTPlayerState* FindTopTeamScoreFor(uint8 TeamNum);

	virtual void DrawStatsLeft(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom) override;
};
