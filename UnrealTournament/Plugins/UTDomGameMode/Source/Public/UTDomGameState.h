// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "MultiTeamGameState.h"
#include "ControlPoint.h"
#include "UTADomTypes.h"
#include "UTDomGameState.generated.h"

UCLASS()
class UTDOMGAMEMODE_API AUTDomGameState : public AMultiTeamGameState
{
	GENERATED_UCLASS_BODY()

	/** Array of The control points */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = GameStateDOM)
	TArray<AControlPoint*> GameControlPoints;

	/** 
	 * The main color used in each teams material skins. 
	 * Replicated Value is set by UTDomGameMode::InitGameState() 
	 * Array index == TeamNum
	 */
	//UPROPERTY(Replicated, EditAnywhere, Category = TeamSkins)
	//FLinearColor TeamBodySkinColor[4];

	/**
	 * The team overlay color used in each teams material skins. 
	 * Replicated Value is set by UTDomGameMode::InitGameState() 
	 * Array index == TeamNum
	 */
	//UPROPERTY(Replicated, EditAnywhere, Category = TeamSkins)
	//FLinearColor TeamSkinOverlayColor[4];

	UPROPERTY(Replicated)
	bool bIsDDOMGame;

//private:
//	/**
//	 * Used to limit updating team skins to every other tick
//	 */
//	UPROPERTY(Transient)
//	bool bSkipThisTick;

public:

	virtual TEnumAsByte<EControlPoint::Type> GetControlPointType();

	/**
	 * Registers the control point described by DomObj.
	 * @param	DomObj			the AControlPoint.
	 * @param	bIsDisabled		True=Disabling the point in parameter DomObj.
	 */
	virtual void RegisterControlPoint(AControlPoint* DomObj, bool bIsDisabled);

	/**
	 * Updates the control point effects on clients.
	 * @param	ThePoint		the point.
	 * @param	NewTeamIndex	Zero-based index of the new team.
	 */
	UFUNCTION(BlueprintCallable, Category = GameStateDOM)
	virtual void UpdateControlPointFX(AControlPoint* ThePoint, uint8 NewTeamIndex);

	/**
	 * Finds the current winning team, or NULL if tied.
	 * @return	AMultiTeamTeamInfo	The Current winning team or NULL if tied at zero
	 */
	//UFUNCTION(BlueprintCallable, Category = GameStateDOM)
	virtual AMultiTeamTeamInfo* FindLeadingTeam() override;

	///**
	// * Finds the player with the highest score from the team specified in TeamNumToTest
	// * @param	TeamNumToTest	TeamIndex of the team to check.
	// * @return	AUTPlayerState	The player with highest score
	// */
	//UFUNCTION(BlueprintCallable, Category = GameStateDOM)
	//virtual AUTPlayerState* FindBestPlayerOnTeam(uint8 TeamNumToTest);

	/**
	 * Gets array of control points.
	 * @return	array of control points.
	 */
	UFUNCTION(BlueprintCallable, Category = GameStateDOM)
	const TArray<AControlPoint*>& GetControlPoints() { return GameControlPoints; };

	/** 
	 * Returns the TeamBodySkinColor for the given TeamIndex 
	 * @param	TeamIndex		TeamIndex of the team to lookup
	 * @return	FLinearColor	The TeamBodySkinColor for the givin TeamIndex
	 */
	//UFUNCTION(BlueprintCallable, Category = TeamSkins)
	//inline FLinearColor GetTeamSkinColor(uint8 TeamIndex) const { return TeamBodySkinColor[TeamIndex]; };

	///** 
	// * Returns the TeamSkinOverlayColor for the given TeamIndex 
	// * @param	TeamIndex		TeamIndex of the team to lookup
	// * @return	FLinearColor	The TeamSkinOverlayColor for the givin TeamIndex
	// */
	//UFUNCTION(BlueprintCallable, Category = TeamSkins)
	//inline FLinearColor GetTeamSkinOverlayColor(uint8 TeamIndex) const { return TeamSkinOverlayColor[TeamIndex]; };

	/** Used by DDOM */
	virtual void SendDDomMessage(int32 MsgIndex, UObject* OptionalObj) {};

	//virtual void SetWinner(AUTPlayerState* NewWinner) override;
	//virtual FText GetGameStatusText(bool bForScoreboard = false) override;

	//virtual void Tick(float DeltaTime) override;
	virtual void UpdateHighlights_Implementation() override;
};
