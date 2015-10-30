/**
* Domination (A.K.A. Classic Domination) Game Mode.
* Based of the original UT:GoTY version of Domination.
* Created by Brian 'Snake' Alexander, 2015
*/
#pragma once

#include "UnrealTournament.h"
#include "UTDomGameMode.h"
#include "ControlPoint.h"
#include "Domination.generated.h"

UCLASS(Config = UTDomGameMode)
class ADomination : public AUTDomGameMode
{
	GENERATED_UCLASS_BODY()

	/**
	* Removes all but one control point from the game
	*/
	UPROPERTY(Config)
	uint32 bKingOfTheHill : 1;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;

	/**
	* Logic to register a found ControlPoint actor with the game and gamestate.
	* @param	DomObj	The ControlPoint to register
	*/
	virtual void RegisterGameControlPoint(AControlPoint* DomObj) override;
	virtual void DefaultTimer();

	/**
	* Awards any controllingTeams TeamScoreAmount amount of points
	* Called every tick of gameplay
	* @param	ControlPointIndex	Index value of the ControlPoint in the CDomPoints array
	* @param	TeamScoreAmount		Amount of points to award to each team
	*/
	virtual void ScoreTeam(uint8 ControlPointIndex, float TeamScoreAmount);
};
