// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "UTDomGameState.generated.h"

UCLASS()
class UTDOMGAMEMODE_API AUTDomGameState : public AUTGameState
{
	GENERATED_UCLASS_BODY()

	/* Array of The control points */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = GameStateDOM)
		TArray<AControlPoint*> GameControlPoints;

	/**
	 * Registers the control point described by DomObj.
	 * @param	DomObj	the AControlPoint.
	 * @param	bIsDisabled	True=Disabling the point in parameter DomObj.
	 */
	virtual void RegisterControlPoint(AControlPoint* DomObj, bool bIsDisabled);

	/**
	 * Updates the control point effects on clients.
	 * @param	ThePoint	the point.
	 * @param	NewTeamIndex		Zero-based index of the new team.
	 */
	UFUNCTION(BlueprintCallable, Category = GameStateDOM)
	virtual void UpdateControlPointFX(AControlPoint* ThePoint, uint8 NewTeamIndex);

	/**
	* Finds the current winning team, or NULL if tied.
	* @return	AUTTeamInfo	The Current winning team
	*/
	UFUNCTION(BlueprintCallable, Category = GameStateDOM)
	virtual AUTTeamInfo* FindLeadingTeam();

	/**
	* Gets the team that is not the WinningTeamIndex and returns the other teams score
	* @return	Team Score
	* @note		This is not 4 team compatible, returns the 1st, teamIndex != WinningTeamIndex
	*/
	UFUNCTION(BlueprintCallable, Category = GameStateDOM)
	virtual int32 GetOtherTeamScore(uint8 WinningTeamIndex) const;

	/**
	* Finds the player with the highest score from the team specified in TeamNumToTest
	* @param	TeamNumToTest	TeamIndex of the team to check.
	* @return	AUTPlayerState	The player with highest score
	*/
	UFUNCTION(BlueprintCallable, Category = GameStateDOM)
	virtual AUTPlayerState* FindBestPlayerOnTeam(uint8 TeamNumToTest);

	/**
	 * Gets array of control points.
	 * @return	array of control points.
	 */
	UFUNCTION(BlueprintCallable, Category = GameStateDOM)
	const TArray<AControlPoint*>& GetControlPoints() { return GameControlPoints; };

	virtual void SetWinner(AUTPlayerState* NewWinner) override;
	virtual FText GetGameStatusText(bool bForScoreboard = false) override;
};
