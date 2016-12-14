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
	UPROPERTY(EditAnywhere, Category = "Scoreboard")
	FVector2D DOMTeamIconUV[5];

	virtual FLinearColor GetBaseHUDColor() override;
	FLinearColor GetWidgetTeamColor();
	virtual FColor GetColorByTeamIndex(int32 TeamIndex);
	virtual FLinearColor GetLinearColorByTeamIndex(int32 TeamIndex);
	virtual void DrawMinimapSpectatorIcons() override;
};
