// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "UTDomGameState.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(UTDomGameState, Log, All);

UCLASS()
class AUTDomGameState : public AUTGameState
{
	GENERATED_UCLASS_BODY()

	/* Array of The control points */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = GameStateDOM)
		TArray<AControlPoint*> GameControlPoints;

	UPROPERTY(Replicated)
		bool KingOfTheHill;

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
		virtual void UpdateControlPointFX(AControlPoint* ThePoint, int32 NewTeamIndex);

	/**
	* Finds the current winning team, or NULL if tied.
	* @return	AUTTeamInfo	The Current winning team
	*/
	UFUNCTION(BlueprintCallable, Category = GameStateDOM)
		virtual AUTTeamInfo* FindLeadingTeam();

	UFUNCTION(BlueprintCallable, Category = GameStateDOM)
		virtual AUTPlayerState* FindBestPlayerOnTeam(int32 TeamNumToTest);

	/**
	 * Gets array of control points.
	 * @return	array of control points.
	 */
	UFUNCTION(BlueprintCallable, Category = GameStateDOM)
		const TArray<AControlPoint*>& GetControlPoints() { return GameControlPoints; };

	virtual void SetWinner(AUTPlayerState* NewWinner) override;
};
