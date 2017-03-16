// Created by Brian 'Snake' Alexander, 2017
#pragma once

#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTADomTypes.h"
#include "UTDoubleDomGameState.generated.h"

UCLASS()
class UTDOMGAMEMODE_API AUTDoubleDomGameState : public AUTDomGameState
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	TSubclassOf<class UUTLocalMessage> DDomMessageClass;

	/** 
	 * Counts the number of control points spawned, and used by GetControlPointType()
	 * to setup xDomPointA and xDomPointB for the 1st 2 and then disable any more.
	 */
	UPROPERTY(Transient)
	int8 PreSpawnControlPointCount;

	virtual void SendDDomMessage(int32 MsgIndex, UObject* OptionalObj) override;
	virtual TEnumAsByte<EControlPoint::Type> GetControlPointType() override;
};
