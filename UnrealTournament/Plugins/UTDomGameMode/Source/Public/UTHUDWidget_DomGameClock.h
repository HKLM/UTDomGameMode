// Adds 4 team score display on HUD
// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTHUDWidget_TeamGameClock.h"
#include "UTHUDWidget_DomGameClock.generated.h"

UCLASS()
class UUTHUDWidget_DomGameClock : public UUTHUDWidget_TeamGameClock
{
	GENERATED_UCLASS_BODY()

	virtual void InitializeWidget(AUTHUD* Hud);
	virtual void Draw_Implementation(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard")
		UTexture2D* HUDDomAtlas;
};
