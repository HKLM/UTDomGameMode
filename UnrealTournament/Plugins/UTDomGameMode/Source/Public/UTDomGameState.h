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

	UPROPERTY(Replicated)
	bool bIsDDOMGame;

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
	virtual AMultiTeamTeamInfo* FindLeadingTeam() override;

	/**
	 * Gets array of control points.
	 * @return	array of control points.
	 */
	UFUNCTION(BlueprintCallable, Category = GameStateDOM)
	const TArray<AControlPoint*>& GetControlPoints() { return GameControlPoints; };

	/** Used by DDOM */
	virtual void SendDDomMessage(int32 MsgIndex, UObject* OptionalObj) {};

	virtual void UpdateHighlights_Implementation() override;
};
