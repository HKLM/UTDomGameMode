// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTLocalMessage.h"
#include "UTDoubleDomGameMessage.h"
#include "UTAnnouncer.h"
#include "ControlPoint.h"

UUTDoubleDomGameMessage::UUTDoubleDomGameMessage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MessageArea = FName(TEXT("GameMessages"));
	ControlledByTeam = NSLOCTEXT("UTDoubleDomGameMessage", "ControlledByTeam", "Control Point [{OptionalControlPoint}] now controlled by {OptionalTeam} Team!");
	ControlsBothPointsString = NSLOCTEXT("UTDoubleDomGameMessage", "ControlsBothPointsString", "{OptionalTeam} Team controls both points!");
	NewRoundIn = NSLOCTEXT("UTDoubleDomGameMessage", "NewRoundIn", "New round in");
	AvertedText = NSLOCTEXT("UTDoubleDomGameMessage", "AvertedText", "Averted");
	bIsStatusAnnouncement = true;
	bIsPartiallyUnique = true;

	DDTimerText.Add(NSLOCTEXT("UTDoubleDomGameMessage", "Text1", "1..."));
	DDTimerText.Add(NSLOCTEXT("UTDoubleDomGameMessage", "Text2", "2..."));
	DDTimerText.Add(NSLOCTEXT("UTDoubleDomGameMessage", "Text3", "3..."));
	DDTimerText.Add(NSLOCTEXT("UTDoubleDomGameMessage", "Text4", "4..."));
	DDTimerText.Add(NSLOCTEXT("UTDoubleDomGameMessage", "Text5", "5..."));
	DDTimerText.Add(NSLOCTEXT("UTDoubleDomGameMessage", "Text6", "6..."));
	DDTimerText.Add(NSLOCTEXT("UTDoubleDomGameMessage", "Text7", "7..."));
	DDTimerText.Add(NSLOCTEXT("UTDoubleDomGameMessage", "Text8", "8..."));
	DDTimerText.Add(NSLOCTEXT("UTDoubleDomGameMessage", "Text9", "9..."));
	DDTimerText.Add(NSLOCTEXT("UTDoubleDomGameMessage", "Text10", "10..."));
}

FLinearColor UUTDoubleDomGameMessage::GetMessageColor(int32 MessageIndex) const
{
	return FLinearColor::Yellow;
}

FText UUTDoubleDomGameMessage::GetText(int32 Switch, bool bTargetsPlayerState1, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, UObject* OptionalObject) const
{
	switch (Switch)
	{
		case 1: return DDTimerText[0]; break;
		case 2: return DDTimerText[1]; break;
		case 3: return DDTimerText[2]; break;
		case 4: return DDTimerText[3]; break;
		case 5: return DDTimerText[4]; break;
		case 6: return DDTimerText[5]; break;
		case 7: return DDTimerText[6]; break;
		case 8: return DDTimerText[7]; break;
		case 9: return DDTimerText[8]; break;
		case 10: return DDTimerText[9]; break;
		case 11: return DDTimerText[10]; break;
		case 12: return ControlledByTeam; break;
		case 13: return ControlsBothPointsString; break;
		case 14: return NewRoundIn; break;
		case 15: return AvertedText; break;
	}

	return FText::GetEmpty();
}

void UUTDoubleDomGameMessage::GetArgs(FFormatNamedArguments& Args, int32 Switch, bool bTargetsPlayerState1, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, UObject* OptionalObject) const
{
	Args.Add(TEXT("Player1Name"), RelatedPlayerState_1 != NULL ? FText::FromString(RelatedPlayerState_1->PlayerName) : FText::GetEmpty());
	Args.Add(TEXT("Player1Score"), RelatedPlayerState_1 != NULL ? FText::AsNumber(int32(RelatedPlayerState_1->Score)) : FText::GetEmpty());
	Args.Add(TEXT("Player2Name"), RelatedPlayerState_2 != NULL ? FText::FromString(RelatedPlayerState_2->PlayerName) : FText::GetEmpty());
	Args.Add(TEXT("Player2Score"), RelatedPlayerState_2 != NULL ? FText::AsNumber(int32(RelatedPlayerState_2->Score)) : FText::GetEmpty());

	AUTPlayerState* UTPlayerState = Cast<AUTPlayerState>(RelatedPlayerState_1);
	Args.Add(TEXT("Player1Team"), (UTPlayerState && UTPlayerState->Team) ? UTPlayerState->Team->TeamName : FText::GetEmpty());

	UTPlayerState = Cast<AUTPlayerState>(RelatedPlayerState_2);
	Args.Add(TEXT("Player2Team"), (UTPlayerState && UTPlayerState->Team) ? UTPlayerState->Team->TeamName : FText::GetEmpty());

	UClass* DamageTypeClass = Cast<UClass>(OptionalObject);
	if (DamageTypeClass != NULL && DamageTypeClass->IsChildOf(UUTDamageType::StaticClass()))
	{
		UUTDamageType* DamageType = DamageTypeClass->GetDefaultObject<UUTDamageType>();
		if (DamageType != NULL)
		{
			Args.Add(TEXT("WeaponName"), DamageType->AssociatedWeaponName);
		}
	}
	if (Cast<AControlPoint>(OptionalObject))
	{
		AControlPoint* CP = Cast<AControlPoint>(OptionalObject);
		Args.Add(TEXT("OptionalControlPoint"), CP ? FText::FromString(CP->PointName) : FText::GetEmpty());
		Args.Add(TEXT("OptionalTeam"), CP->ControllingTeam ? CP->ControllingTeam->TeamName : FText::GetEmpty());
	}
	else if (Cast<AUTTeamInfo>(OptionalObject))
	{
		AUTTeamInfo* TeamInfo = Cast<AUTTeamInfo>(OptionalObject);
		Args.Add(TEXT("OptionalTeam"), TeamInfo ? TeamInfo->TeamName : FText::GetEmpty());
	}
}

bool UUTDoubleDomGameMessage::InterruptAnnouncement_Implementation(int32 Switch, const UObject* OptionalObject, TSubclassOf<UUTLocalMessage> OtherMessageClass, int32 OtherSwitch, const UObject* OtherOptionalObject) const
{
	if (Cast<UUTLocalMessage>(OtherMessageClass->GetDefaultObject())->bOptionalSpoken)
	{
		return true;
	}
	if (GetClass() == OtherMessageClass)
	{
		//TODO 
		//if ((OtherSwitch == 0) || (OtherSwitch == 1) || (OtherSwitch == 2) || (OtherSwitch == 3))
		//{
		//	// never interrupt scoring announcements
		//	return false;
		//}
		if (OptionalObject == OtherOptionalObject)
		{
			// interrupt announcement about same object
			return true;
		}
	}
	return false;
}

FName UUTDoubleDomGameMessage::GetTeamAnnouncement(int32 Switch, uint8 TeamNum) const
{
	switch (Switch)
	{
		case 1: return TEXT("CD1"); break;
		case 2: return TEXT("CD2"); break;
		case 3: return TEXT("CD3"); break;
		case 4: return TEXT("CD4"); break;
		case 5: return TEXT("CD5"); break;
		case 6: return TEXT("CD6"); break;
		case 7: return TEXT("CD7"); break;
		case 8: return TEXT("CD8"); break;
		case 9: return TEXT("CD9"); break;
		case 10: return TEXT("CD10"); break;
		case 13: return TeamNum == 0 ? TEXT("RedDominating") : TEXT("BlueDominating"); break;
		case 14: return TEXT("NewRoundIn"); break;
		case 15: return TEXT("Averted"); break;
	}
	return NAME_None;
}

FName UUTDoubleDomGameMessage::GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject) const
{
	const AUTTeamInfo* TeamInfo = Cast<AUTTeamInfo>(OptionalObject);
	uint8 TeamNum = (TeamInfo != NULL) ? TeamInfo->GetTeamNum() : 0;

	return GetTeamAnnouncement(Switch, TeamNum);
}
