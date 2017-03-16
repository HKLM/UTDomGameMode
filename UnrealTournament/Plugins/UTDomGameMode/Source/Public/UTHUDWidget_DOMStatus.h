// Created by Brian 'Snake' Alexander, 2015
// Draws the HUD icons for each ControlPoint
#pragma once

#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "UTDomGameState.h"
#include "UTHUDWidget_DOMStatus.generated.h"

USTRUCT()
struct FPointInfo
{
	GENERATED_USTRUCT_BODY()

	/** The current position on HUD to draw this at */
	FVector2D StatusIcon;

	/** The current ControlPoint associated with this */
	AControlPoint* thePoint;

	/** Directional arrow pointing in the direction of this ControlPoint from the players current location */
	FHUDRenderObject_Texture DomArrowDir;
};

UCLASS()
class UTDOMGAMEMODE_API UUTHUDWidget_DOMStatus : public UUTHUDWidget
{
	GENERATED_UCLASS_BODY()

	/** The control points */
	TArray<FPointInfo> CtrlPoints;

	/** Temp value of location on screen to draw the current ControlPoint icon */
	FVector2D DomPosition;

	/** array of dom team icon HUD texture */
	TArray<UTexture2D*> DomTeamIconTexture;

	TArray<UTexture2D*> xDDomTeamIconTexture;

	/** draw direction arrow (ArrowDirTexture) for each control point on HUD */
	bool bDrawDirectionArrow;

	/**
	 * colors assigned to the Directional Arrow on a per team bases. This way we can change the arrow color depending on team.
	 * (e.g. A red arrow on a red team logo would not be visible.
	 */
	TArray<FLinearColor> ArrowDirColor;

	UPROPERTY(Transient)
		bool bControlPointInitialized;

	/** Base size of HUD icon for each control point */
	float IcoMulti;
	float IconSize;

	/**
	 * used as a quick way not to have to recalculate the icons.
	 * Copy of RenderScale to compair this time against. If differant then we will have to resetup the icons 
	 */
	float LastRenderScale;

	virtual void Draw_Implementation(float DeltaTime);
	virtual void InitializeWidget(AUTHUD* Hud);

	virtual bool ShouldDraw_Implementation(bool bShowScores)
	{
		return !bShowScores;
	}

	/** Cache the ControlPoints */
	void FindControlPoints();

	/**
	 * Gets the texture of the givin Team number
	 * @param TeamIndex The TeamIndex of team to look up 
	 */
	UTexture2D* GetDomTeamIcon(uint8 TeamIndex) const;

	/**
	 * Gets the TeamColor of the givin Team number
	 * @param TeamIndex The TeamIndex of team to look up 
	 */
	FLinearColor GetDomTeamColor(uint8 TeamIndex) const;

protected:
	
		/**
		 * Cached pointer to UTDomGameState 
		 */
		AUTDomGameState* DomGameState;

		UPROPERTY(Transient)
		bool bIsDoubleDom;

};
