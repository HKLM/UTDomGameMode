/**
 * Domination (A.K.A. Classic Domination) Game Mode.
 * Based of the original UT:GoTY version of Domination.
 * Created by Brian 'Snake' Alexander, 2015
 */
#pragma once

#include "UnrealTournament.h"
#include "UTDomGameMode.h"
#include "Domination.generated.h"

UCLASS()
class UTDOMGAMEMODE_API ADomination : public AUTDomGameMode
{
	GENERATED_UCLASS_BODY()

	virtual void DefaultTimer() override;

	/**
	 * Awards any controllingTeams TeamScoreAmount amount of points
	 * Called every tick of gameplay
	 * @param	ControlPointIndex	Index value of the ControlPoint in the CDomPoints array
	 * @param	TeamScoreAmount		Amount of points to award to each team
	 */
	virtual void ScoreTeam(uint8 ControlPointIndex, float TeamScoreAmount);
};
