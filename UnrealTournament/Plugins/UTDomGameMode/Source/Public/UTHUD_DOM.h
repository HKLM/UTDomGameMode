// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTHUD.h"
#include "UTHUD_DOM.generated.h"

UCLASS()
class UTDOMGAMEMODE_API AUTHUD_DOM : public AUTHUD
{
	GENERATED_UCLASS_BODY()

	// Icons to display on minimap
	UPROPERTY(EditAnywhere, Category = "HUD")
	FVector2D DOMTeamIconUV[5];

	// Custom HUDAtlas based off original, but adds 4 team logos
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard")
	UTexture2D* HUDDomAtlas;

	virtual void BeginPlay() override;
	virtual FLinearColor GetBaseHUDColor() override;
	FLinearColor GetWidgetTeamColor();

	/**
	 * Lookup FColor for the givin Team
	 * @param	TeamIndex	the team to lookup
	 * @return	FColor		color of the team
	 */
	virtual FColor GetColorByTeamIndex(int32 TeamIndex);

	/**
	 * Lookup FLinearColor for the givin Team
	 * @param	TeamIndex		the team to lookup
	 * @return	FLinearColor	color of the team
	 */
	virtual FLinearColor GetLinearColorByTeamIndex(int32 TeamIndex);

	virtual void DrawMinimapSpectatorIcons() override;
};
