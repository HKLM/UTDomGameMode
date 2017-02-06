/**
 * Adds 4 team support
 * Created by Brian 'Snake' Alexander, 2016
 */
#pragma once

#include "UnrealTournament.h"
#include "UTPlayerState.h"
#include "UTDomPlayerState.generated.h"

UCLASS()
class UTDOMGAMEMODE_API AUTDomPlayerState : public AUTPlayerState
{
	GENERATED_UCLASS_BODY()

	/**
	 * Logic to make stock character skins into bright team skins for 4 teams (Red, Blue, Green, Gold)
	 * @param	NewTeamIndex	The TeamNum of team to color players skin to
	 * @return	bool			if skin was made sucessfully
	 */
	UFUNCTION(BlueprintCallable, Category = TeamSkins)
	virtual void MakeTeamSkin(uint8 NewTeamIndex);

	virtual void HandleTeamChanged(AController* Controller) override;
	virtual void ServerRequestChangeTeam_Implementation(uint8 NewTeamIndex) override;
};
