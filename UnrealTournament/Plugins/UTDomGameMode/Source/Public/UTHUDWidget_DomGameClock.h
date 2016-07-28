// Created by Brian 'Snake' Alexander, (c) 2016
#pragma once

#include "UnrealTournament.h"
#include "UTHUDWidget_TeamGameClock.h"
#include "UTHUDWidget_DomGameClock.generated.h"

UCLASS()
class UUTHUDWidget_DomGameClock : public UUTHUDWidget_TeamGameClock
{
	GENERATED_UCLASS_BODY()

	virtual void InitializeWidget(AUTHUD* Hud);
	virtual void Draw_Implementation(float DeltaTime) override;

	UTexture2D* HUDDomAtlas;
};
