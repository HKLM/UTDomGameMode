/**
* Double Domination Game Mode.
* Based of the original UT2k3 version, by EPIC games
* Created by Brian 'Snake' Alexander, 2015
**/
#pragma once

#include "UnrealTournament.h"
#include "UTDomGameMode.h"
#include "UTDoubleDomGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(UTDoubleDomGameMode, Log, All);

UCLASS(Blueprintable, Config = UTDomGameMode)
class AUTDoubleDomGameMode : public AUTDomGameMode
{
	GENERATED_UCLASS_BODY()

	// Override for maps that do not have team player starts
	UPROPERTY(Config)
		uint32 bNoTeamPlayerStart:1;

	// The amount of time required to hold both control points to score
	UPROPERTY(Config, BlueprintReadOnly, Category = DDOM)
		int32 TimeToScore;

	static const int32 MAXTIMETOSCORE;
	static const int32 MAXTIMEDISABLED;

	// The amount of time after a team has scored until the next round begins
	UPROPERTY(Config, BlueprintReadOnly, Category = DDOM)
		int32 TimeDisabled;

	// Temp count down number from TimeToScore
	UPROPERTY(Transient, BlueprintReadOnly, Category = DDOM)
		int32 ScoreCountDown;

	// Temp count down from TimeDisabled
	UPROPERTY(Transient, BlueprintReadOnly, Category = DDOM)
		int32 DisabledCountDown;

	UPROPERTY(Transient)
		bool bPlayedDisabledMsg;

	UPROPERTY(Transient)
		bool bPlayedTeamDominating;

	/**
	* The sound to play when this point is captured
	**/
	UPROPERTY(BlueprintReadWrite, Category = DDOM)
		USoundBase* AwardPointSound;

	UFUNCTION()
		virtual void ResetCount();

	virtual void RegisterGameControlPoint(ADominationObjective* DomObj) override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	//virtual void PreInitializeComponents() override;
	virtual void InitGameState() override;
	virtual void GameObjectiveInitialized(AUTGameObjective* Obj) override;
	virtual void DefaultTimer() override;
	void ScoreDDomTeam();
	virtual bool CheckScore_Implementation(AUTPlayerState* Scorer) override;
	virtual void CheckGameTime() override;
	virtual void BroadcastLocalized(AActor* Sender, TSubclassOf<ULocalMessage> Message, int32 Switch = 0, APlayerState* RelatedPlayerState_1 = NULL, APlayerState* RelatedPlayerState_2 = NULL, UObject* OptionalObject = NULL) override;
};
