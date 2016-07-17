// Created by Brian 'Snake' Alexander, (c) 2016
#pragma once

#include "UnrealTournament.h"
#include "SiegePoint.h"
#include "SiegeGameState.h"
#include "UTHUDWidget_SiegeStatus.generated.h"

USTRUCT()
struct FSiegePointInfo
{
	GENERATED_USTRUCT_BODY()

	/* The current position on HUD to draw this at */
	FVector2D StatusIcon;

	/* The current SiegePoint associated with this */
	ASiegePoint* thePoint;

	/* Directional arrow pointing in the direction of this SiegePoint from the players current location */
	FHUDRenderObject_Texture DomArrowDir;
};

UCLASS()
class UUTHUDWidget_SiegeStatus : public UUTHUDWidget
{
	GENERATED_UCLASS_BODY()

	/* colors assigned to the teams */
	TArray<FLinearColor> TeamColors;

	/* The control points */
	TArray<FSiegePointInfo> CtrlPoints;

	/* Temp value of location on screen to draw the current SiegePoint icon */
	FVector2D DomPosition;

	/* array of dom team icon HUD texture */
	TArray<UTexture2D*> DomTeamIconTexture;

	/* draw direction arrow (ArrowDirTexture) for each control point on HUD */
	bool bDrawDirectionArrow;

	/* Our HUDAtlas texture that has a white arrow so we can color it dynamicly */
	UTexture2D* ArrowDirTexture;
	/**
	* colors assigned to the Directional Arrow on a per team bases. This way we can change the arrow color depending on team.
	* (e.g. A red arrow on a red team logo would not be visible.
	*/
	TArray<FLinearColor> ArrowDirColor;

	UPROPERTY(Transient)
		bool bSiegePointInitialized;

	/* Base size of HUD icon for each control point */
	float IcoMulti;
	float IconSize;

	/* used as a quick way not to have to recalculate the icons.
	* Copy of RenderScale to compair this time against. If differant then we will have to resetup the icons */
	float LastRenderScale;

	virtual void Draw_Implementation(float DeltaTime);
	virtual void InitializeWidget(AUTHUD* Hud);

	virtual bool ShouldDraw_Implementation(bool bShowScores)
	{
		return !bShowScores;
	}

	/* Cache the SiegePoints */
	void FindSiegePoints();

	/* Gets the texture of the givin Team number
	* @param TeamIndex The TeamIndex of team to look up */
	UTexture2D* GetDomTeamIcon(uint8 TeamIndex) const;
};
