// Created by Brian 'Snake' Alexander, 2017

#include "UnrealTournament.h"
#include "UTLocalMessage.h"
#include "UTVictoryMessage.h"
#include "UTDomVictoryMessage.h"
//#include "UTHUD.h"
#include "GameFramework/LocalMessage.h"

UUTDomVictoryMessage::UUTDomVictoryMessage(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GreenTeamName = NSLOCTEXT("DomGameMessage", "GreenTeamName", "GREEN TEAM");
	GoldTeamName = NSLOCTEXT("DomGameMessage", "GoldTeamName", "GOLD TEAM");
}

FName UUTDomVictoryMessage::GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject, const class APlayerState* RelatedPlayerState_1, const class APlayerState* RelatedPlayerState_2) const
{
	if (Switch == 3)
	{
		return FName(TEXT("FlawlessVictory"));
	}

	const AUTTeamInfo* WinningTeam = Cast<AUTTeamInfo>(OptionalObject);
	if (WinningTeam != nullptr)
	{
		FName outtxt;
		switch (WinningTeam->TeamIndex)
		{
			case 0: outtxt = FName(TEXT("RedTeamIsTheWinner")); break;
			case 1: outtxt = FName(TEXT("BlueTeamIsTheWinner")); break;
			case 2: outtxt = FName(TEXT("GreenTeamIsTheWinner")); break;
			case 3: outtxt = FName(TEXT("GoldTeamIsTheWinner")); break;
			default: break;
		}
		return outtxt;
	}
	else
	{
		return ((Switch == 1) || (Switch == 5)) ? FName(TEXT("WonMatch")) : FName(TEXT("LostMatch"));
	}
}

void UUTDomVictoryMessage::GetEmphasisText(FText& PrefixText, FText& EmphasisText, FText& PostfixText, FLinearColor& EmphasisColor, int32 Switch, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const
{
	PrefixText = TeamWinsPrefix;
	PostfixText = TeamWinsPostfix;
	AUTTeamInfo* WinningTeam = Cast<AUTTeamInfo>(OptionalObject);
	if (WinningTeam != nullptr)
	{
		EmphasisText = WinningTeam->TeamName;
		EmphasisColor = WinningTeam->TeamColor;

		if (Switch >= 4)
		{
			PrefixText = TeamWinsSecondaryPrefix;
			PostfixText = TeamWinsSecondaryPostfix;
		}
		return;
	}
	else if ((RelatedPlayerState_1 != RelatedPlayerState_2) && (RelatedPlayerState_1 != nullptr))
	{
		EmphasisText = FText::FromString(RelatedPlayerState_1->PlayerName);
		EmphasisColor = FLinearColor::Red;
		return;
	}

	Super::UUTLocalMessage::GetEmphasisText(PrefixText, EmphasisText, PostfixText, EmphasisColor, Switch, RelatedPlayerState_1, RelatedPlayerState_2, OptionalObject);
}
