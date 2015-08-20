// Copyright 2007-2015 Brian 'Snake' Alexander, All Rights Reserved.

#include "UnrealTournament.h"
#include "UTDoubleDomGameState.h"
#include "UTDoubleDomGameMessage.h"
#include "xDomPoint.h"

AUTDoubleDomGameState::AUTDoubleDomGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DDomMessageClass = UUTDoubleDomGameMessage::StaticClass();
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
