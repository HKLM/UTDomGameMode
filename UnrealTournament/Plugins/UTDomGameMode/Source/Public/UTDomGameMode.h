/**
* Base class for Domination game modes
* Created by Brian 'Snake' Alexander, 2015
**/
#pragma once

#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTDomTeamInfo.h"
//#include "UTDomStats.h"
#include "ControlPoint.h"
#include "DominationObjective.h"
#include "UTDomGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(UTDomGameMode, Log, All);

UCLASS(Abstract, Config = UTDomGameMode)
class AUTDomGameMode : public AUTTeamGameMode
{
	GENERATED_UCLASS_BODY()

	/** Array of the ControlPoints for the map */
	UPROPERTY(BlueprintReadOnly, Category = DOM)
		TArray<AControlPoint*> CDomPoints;

	/** The game modes ControlPoint type to spawn */
	UPROPERTY()
		TSubclassOf<class ABaseControlPoint> DominationObjectiveType;

	UPROPERTY(BlueprintReadOnly, Category = DOM)
		AUTDomGameState* DomGameState;

	/** Class of DomStats associated with this GameMode. */
	//UPROPERTY(EditAnywhere, noclear, BlueprintReadWrite, Category = Classes)
	//	TSubclassOf<class AUTDomStats> DomStatsClass;

	/** Handles individual player stats */
	//UPROPERTY(BlueprintReadOnly, Category = DOM)
	//	AUTDomStats* DomStats;

	/** Allow the use of the translocator */
	UPROPERTY(Config)
		uint32 bAllowTranslocator : 1;

	/** Max allowed number of Control Points allowed */
	UPROPERTY(Config)
		int32 MaxControlPoints;

	/**
	* Adds the ADominationObjective->MyControlPoint to the CDomPoints array
	* @param	DomObj	ADominationObjective
	*/
	virtual void RegisterGameControlPoint(ADominationObjective* DomObj);
	virtual void PreInitializeComponents() override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual void BeginPlay() override;
	virtual void GameObjectiveInitialized(AUTGameObjective* Obj) override;
	virtual bool CheckScore_Implementation(AUTPlayerState* Scorer) override;
	virtual void SetEndGameFocus(AUTPlayerState* Winner) override;
	virtual void PlacePlayersAroundDomBase(int32 TeamNum, AControlPoint* DomBase);
	virtual void Logout(AController* Exiting) override;

	/**
	* Called when a player leaves the match
	* @param	ControllingPawn		The leaving player
	**/
	virtual void ClearControl(AUTPlayerState* ControllingPawn);

	virtual void ScoreKill_Implementation(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType) override;
	//void BuildServerResponseRules(FString& OutRules);
};
