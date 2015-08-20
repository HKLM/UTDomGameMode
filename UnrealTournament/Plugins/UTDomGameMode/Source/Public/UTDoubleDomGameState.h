// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTDoubleDomGameState.generated.h"

UCLASS()
class AUTDoubleDomGameState : public AUTDomGameState
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
		TSubclassOf<class UUTLocalMessage> DDomMessageClass;

	virtual void SendDDomMessage(int32 MsgIndex, UObject* OptionalObj);
};
