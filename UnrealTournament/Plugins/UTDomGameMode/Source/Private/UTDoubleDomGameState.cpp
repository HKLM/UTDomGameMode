// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTADomTypes.h"
#include "UTDomGameState.h"
#include "UTDoubleDomGameState.h"
#include "UTDoubleDomGameMessage.h"

AUTDoubleDomGameState::AUTDoubleDomGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DDomMessageClass = UUTDoubleDomGameMessage::StaticClass();
	PreSpawnControlPointCount = 0;
	bIsDDOMGame = true;
}

void AUTDoubleDomGameState::SendDDomMessage(int32 MsgIndex, UObject* OptionalObj)
{
	if (GetWorld()->GetNetMode() != NM_DedicatedServer && IsMatchInProgress())
	{
		if (MsgIndex < 16)
		{
			TArray<APlayerController*> PlayerList;
			GEngine->GetAllLocalPlayerControllers(PlayerList);
			for (auto It = PlayerList.CreateIterator(); It; ++It)
			{
				AUTPlayerController* PC = Cast<AUTPlayerController>(*It);
				if (PC != NULL)
				{
					PC->ClientReceiveLocalizedMessage(DDomMessageClass, MsgIndex, NULL, NULL, OptionalObj);
				}
			}
		}
	}
}

TEnumAsByte<EControlPoint::Type> AUTDoubleDomGameState::GetControlPointType()
{
	TEnumAsByte<EControlPoint::Type> retVal;
	switch (PreSpawnControlPointCount)
	{
		case 0:  retVal = EControlPoint::PT_xDomA;    break;
		case 1:  retVal = EControlPoint::PT_xDomB;    break;
		default: retVal = EControlPoint::PT_Disabled; break;
	}
	PreSpawnControlPointCount++;
	return retVal;
}
