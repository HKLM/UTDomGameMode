/**
* Domination Game Mode, plays on DM maps
* Control points will be automaticly added to the map at run time.
* Created by Brian 'Snake' Alexander, 2016
*/
#pragma once

#include "UnrealTournament.h"
#include "Domination.h"
#include "DominationDMmap.generated.h"

UCLASS()
class UTDOMGAMEMODE_API ADominationDMmap : public ADomination
{
	GENERATED_UCLASS_BODY()

	virtual void PreInitializeComponents() override;

#if !UE_SERVER
	virtual void CreateConfigWidgets(TSharedPtr<class SVerticalBox> MenuSpace, bool bCreateReadOnly, TArray< TSharedPtr<TAttributePropertyBase> >& ConfigProps, int32 MinimumPlayers) override;
#endif

protected:
	/** Automaticly generate ControlPoints when playing on a DM map */
	UFUNCTION()
	virtual void AutoGenerateControlPoints();
};
