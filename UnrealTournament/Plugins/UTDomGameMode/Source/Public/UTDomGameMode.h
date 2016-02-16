/**
* Base class for Domination game modes
* Created by Brian 'Snake' Alexander, 2015
**/
#pragma once

#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "ControlPoint.h"
#include "UTDomTeamInfo.h"
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
		bool bAllowTranslocator;

	/** Max number of Control Points allowed to be in play. If there are more than this number, they will be disabled and removed from play */
	UPROPERTY(Config)
		int32 MaxControlPoints;

	/**
	* Adds the DomObj to the CDomPoints array
	* @param	DomObj	the AControlPoint to register
	*/
	virtual void RegisterGameControlPoint(AControlPoint* DomObj);

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual void BeginPlay() override;
	virtual void GameObjectiveInitialized(AUTGameObjective* Obj) override;
	virtual void AnnounceMatchStart() override;
	virtual bool CheckScore_Implementation(AUTPlayerState* Scorer) override;
	virtual void ScoreKill_Implementation(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType) override;
	virtual void EndGame(AUTPlayerState* Winner, FName Reason) override;
	virtual void SetEndGameFocus(AUTPlayerState* Winner) override;
	virtual void Logout(AController* Exiting) override;
	virtual void CreateGameURLOptions(TArray<TSharedPtr<TAttributePropertyBase>>& MenuProps) override;
	void BuildServerResponseRules(FString& OutRules);

	/**
	* Called when a player leaves the match
	* @param	ControllingPawn		The leaving player
	**/
	virtual void ClearControl(AUTPlayerState* ControllingPawn);

#if !UE_SERVER
	virtual void CreateConfigWidgets(TSharedPtr<class SVerticalBox> MenuSpace, bool bCreateReadOnly, TArray< TSharedPtr<TAttributePropertyBase> >& ConfigProps) override;
	virtual void BuildScoreInfo(AUTPlayerState* PlayerState, TSharedPtr<class SUTTabWidget> TabWidget, TArray<TSharedPtr<struct TAttributeStat> >& StatList) override;
#endif

};
