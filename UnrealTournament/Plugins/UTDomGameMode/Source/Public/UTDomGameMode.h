/**
* Base class for Domination game modes
* Created by Brian 'Snake' Alexander, 2015
**/
#pragma once

#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "ControlPoint.h"
#include "UTDomGameMode.generated.h"

UCLASS(Abstract, Config = UTDomGameMode)
class AUTDomGameMode : public AUTTeamGameMode
{
	GENERATED_UCLASS_BODY()

	/** Array of the ControlPoints for the map */
	UPROPERTY(BlueprintReadOnly, Category = DOM)
		TArray<AControlPoint*> CDomPoints;

	UPROPERTY(BlueprintReadOnly, Category = DOM)
		AUTDomGameState* DomGameState;

	/** Allow the use of the translocator */
	UPROPERTY(Config)
		uint32 bAllowTranslocator : 1;

	/** Max allowed number of Control Points allowed */
	UPROPERTY(Config)
		int32 MaxControlPoints;

	/**
	* Adds the ADominationObjective->MyControlPoint to the CDomPoints array
	* @param	DomObj	AControlPoint
	*/
	virtual void RegisterGameControlPoint(AControlPoint* DomObj);
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual void BeginPlay() override;
	virtual void GameObjectiveInitialized(AUTGameObjective* Obj) override;
	virtual bool CheckScore_Implementation(AUTPlayerState* Scorer) override;
	virtual void SetEndGameFocus(AUTPlayerState* Winner) override;
	virtual void Logout(AController* Exiting) override;

	/**
	* Called when a player leaves the match
	* @param	ControllingPawn		The leaving player
	**/
	virtual void ClearControl(AUTPlayerState* ControllingPawn);

	virtual void ScoreKill_Implementation(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType) override;
};
