/**
 * Base class for Domination game modes
 * Created by Brian 'Snake' Alexander, 2015
 */
#pragma once

#include "UnrealTournament.h"
#include "MultiTeamGameMode.h"
#include "UTDomGameState.h"
#include "ControlPoint.h"
#include "UTADomTypes.h"
#include "UTDomGameMode.generated.h"

//const int32 MAX_NUM_TEAMS = 4;

UCLASS(Abstract, Config = UTDomGameMode)
class UTDOMGAMEMODE_API AUTDomGameMode : public AMultiTeamGameMode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	TEnumAsByte<EControlPoint::Type> ObjectiveType;

	/** Array of the ControlPoints for the map */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DOM)
	TArray<AControlPoint*> CDomPoints;

	UPROPERTY(BlueprintReadOnly, Category = DOM)
	AUTDomGameState* DomGameState;

	/** Allow the use of the translocator */
	UPROPERTY(Config, EditDefaultsOnly, Category = DOM)
	bool bAllowTranslocator;

	/** Max number of Control Points allowed to be in play. If there are more than this number, they will be disabled and removed from play */
	UPROPERTY(Config, EditDefaultsOnly, Category = DOM)
	int32 MaxControlPoints;

	/** Used by the UI to set the NumTeams value */
	//UPROPERTY(Config, EditDefaultsOnly, Category = DOM)
	//int32 NumOfTeams;

	/** The main color used in each teams material skins. Value is given to UTDomGameState for replication. Array index == TeamNum */
	//UPROPERTY(Config, EditDefaultsOnly, Category = DOM)
	//FLinearColor TeamBodySkinColor[4];
	
	/** The team overlay color used in each teams material skins. Value is given to UTDomGameState for replication. Array index == TeamNum */
	//UPROPERTY(Config, EditDefaultsOnly, Category = DOM)
	//FLinearColor TeamSkinOverlayColor[4];

	TAssetSubclassOf<AUTWeapon> TranslocatorObject;

	/**
	 * Adds the DomObj to the CDomPoints array
	 * @param	DomObj	the AControlPoint actor to register
	 */
	virtual void RegisterGameControlPoint(AControlPoint* DomObj);

	/**
	 * Called when a player leaves the match
	 * @param	ControllingPawn		The leaving player
	 */
	virtual void ClearControl(AUTPlayerState* ControllingPawn);

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	//virtual void AnnounceMatchStart() override;
	virtual void GiveDefaultInventory(APawn* PlayerPawn) override;
	//virtual bool ChangeTeam(AController* Player, uint8 NewTeam = 255, bool bBroadcast = true) override;
	//virtual bool MovePlayerToTeam(AController* Player, AUTPlayerState* PS, uint8 NewTeam) override;
	virtual bool CheckScore_Implementation(AUTPlayerState* Scorer) override;
	virtual void ScoreKill_Implementation(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType) override;
	virtual void EndGame(AUTPlayerState* Winner, FName Reason) override;
	virtual void SetEndGameFocus(AUTPlayerState* Winner) override;
	//virtual void PlayEndOfMatchMessage() override;
	virtual void Logout(AController* Exiting) override;
	virtual void CreateGameURLOptions(TArray<TSharedPtr<TAttributePropertyBase>>& MenuProps) override;
	void BuildServerResponseRules(FString& OutRules) override;

#if !UE_SERVER
	virtual void CreateConfigWidgets(TSharedPtr<class SVerticalBox> MenuSpace, bool bCreateReadOnly, TArray< TSharedPtr<TAttributePropertyBase> >& ConfigProps, int32 MinimumPlayers) override;
	virtual void BuildScoreInfo(AUTPlayerState* PlayerState, TSharedPtr<class SUTTabWidget> TabWidget, TArray<TSharedPtr<struct TAttributeStat> >& StatList) override;
#endif

};
