/**
 * Adds dynamic 4 team skin support
 * Created by Brian 'Snake' Alexander, 2017
 */
#pragma once

#include "UnrealTournament.h"
#include "UTCharacter.h"
#include "UTDomCharacter.generated.h"

UCLASS()
class UTDOMGAMEMODE_API AUTDomCharacter : public AUTCharacter
{
	GENERATED_UCLASS_BODY()

	/**
	 * The team body color used by SetTeamSkin() to make the characters team material skin
	 * Replicated Value is set from UTDomGameState
	 */
	UPROPERTY(Replicated, VisibleAnywhere, Category = TeamSkins)
		FLinearColor TeamBodySkinColor;

	/**
	 * The team overlay color used by SetTeamSkin() to make the characters team material skin.
	 * Replicated Value is set from UTDomGameState
	 */
	UPROPERTY(Replicated, VisibleAnywhere, Category = TeamSkins)
		FLinearColor TeamSkinOverlayColor;

	/**
	 * The team overlay color used to make the characters team material skin.
	 * @param	NewTeamIndex	The TeamIndex of team to make skin color for
	 */
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Setup Team Skin", Tooltip="Set the characters skin color to match the given TeamIndex.", Keywords="teamskin skin"), Category = TeamSkins)
		virtual void SetTeamSkin(uint8 NewTeamIndex);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
