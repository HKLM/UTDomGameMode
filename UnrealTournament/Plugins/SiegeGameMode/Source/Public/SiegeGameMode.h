/**
* Siege game mode
* Created by Brian 'Snake' Alexander, (c) 2016
**/
#pragma once

#include "UnrealTournament.h"
#include "SiegeGameState.h"
#include "SiegePoint.h"
#include "SiegeGameMode.generated.h"

UCLASS(Config = SiegeGameMode)
class ASiegeGameMode : public AUTTeamGameMode
{
	GENERATED_UCLASS_BODY()

	/** Array of the SiegePoints for the map */
	UPROPERTY(BlueprintReadOnly, Category = DOM)
		TArray<ASiegePoint*> CDomPoints;

	UPROPERTY(BlueprintReadOnly, Category = DOM)
		ASiegeGameState* DomGameState;

	/** Allow the use of the translocator */
	UPROPERTY(Config)
		bool bAllowTranslocator;

	/** Max number of Control Points allowed to be in play. If there are more than this number, they will be disabled and removed from play */
	int32 MaxSiegePoints;

	TAssetSubclassOf<AUTWeapon> TranslocatorObject;

	/**
	* Adds the DomObj to the CDomPoints array
	* @param	DomObj	the ASiegePoint to register
	*/
	virtual void RegisterGameSiegePoint(ASiegePoint* DomObj);
	virtual void DefaultTimer();

	/**
	* Awards any controllingTeams TeamScoreAmount amount of points
	* Called every tick of gameplay
	* @param	ControlPointIndex	Index value of the ControlPoint in the CDomPoints array
	* @param	TeamScoreAmount		Amount of points to award to each team
	*/
	virtual void ScoreTeam(uint8 ControlPointIndex, float TeamScoreAmount);

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual void BeginPlay() override;
	virtual void PreInitializeComponents() override;
	virtual void GameObjectiveInitialized(AUTGameObjective* Obj) override;
	virtual void AnnounceMatchStart() override;
	virtual void GiveDefaultInventory(APawn* PlayerPawn) override;
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
	virtual void CreateConfigWidgets(TSharedPtr<class SVerticalBox> MenuSpace, bool bCreateReadOnly, TArray< TSharedPtr<TAttributePropertyBase> >& ConfigProps, int32 MinimumPlayers) override;
	virtual void BuildScoreInfo(AUTPlayerState* PlayerState, TSharedPtr<class SUTTabWidget> TabWidget, TArray<TSharedPtr<struct TAttributeStat> >& StatList) override;
#endif

};
