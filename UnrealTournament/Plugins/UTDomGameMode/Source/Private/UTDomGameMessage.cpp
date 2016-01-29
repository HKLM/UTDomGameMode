// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTLocalMessage.h"
#include "ControlPoint.h"
#include "UTGameMessage.h"
#include "UTDomGameMessage.h"

UUTDomGameMessage::UUTDomGameMessage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MessageArea = FName(TEXT("GameMessages"));
	ControlledByTeam = NSLOCTEXT("UTDomGameMessage", "ControlledByTeam", "Control Point [{OptionalControlPoint}] now controlled by {OptionalTeam} Team!");

	Lifetime = 1.2;
	bIsStatusAnnouncement = true;
	bIsPartiallyUnique = true;
}

FLinearColor UUTDomGameMessage::GetMessageColor_Implementation(int32 MessageIndex) const
{
	switch (MessageIndex)
	{
		case 0: return FLinearColor(0.8f, 0.0f, 0.05f, 1.0f); break;
		case 1: return FLinearColor(0.1f, 0.1f, 0.8f, 1.0f); break;
		case 2: return FLinearColor(0.0f, 0.9f, 0.0f, 1.0f); break;
		case 3: return FLinearColor(0.85f, 0.85f, 0.0f, 1.0f); break;
		case 9: return FLinearColor(0.8f, 0.0f, 0.05f, 1.0f); break;
		case 10: return FLinearColor(0.1f, 0.1f, 0.8f, 1.0f); break;
		case 11: return FLinearColor(0.0f, 0.9f, 0.0f, 1.0f); break;
		case 12: return FLinearColor(0.85f, 0.85f, 0.0f, 1.0f); break;
		default:
			break;
	}
	return FLinearColor::Yellow;
}

FText UUTDomGameMessage::GetText(int32 Switch, bool bTargetsPlayerState1, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, UObject* OptionalObject) const
{
	switch (Switch)
	{
		case 0: return ControlledByTeam; break;
		case 1: return ControlledByTeam; break;
		case 2: return ControlledByTeam; break;
		case 3: return ControlledByTeam; break;
		case 9: return GetDefault<UUTGameMessage>(GetClass())->YouAreOnRed; break;
		case 10: return GetDefault<UUTGameMessage>(GetClass())->YouAreOnBlue; break;
		//case 11: return YouAreOnGreen; break;
		//case 12: return YouAreOnGold; break;

		default:
			break;
	}

	return FText::GetEmpty();
}

void UUTDomGameMessage::GetArgs(FFormatNamedArguments& Args, int32 Switch, bool bTargetsPlayerState1, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, UObject* OptionalObject) const
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
		AUTDomTeamInfo* DTI = Cast<AUTDomTeamInfo>(CP->ControllingTeam);
		if (DTI != NULL)
		Args.Add(TEXT("OptionalTeam"), DTI ? DTI->TeamName : FText::GetEmpty());
	}
	else
	{
		AUTTeamInfo* TeamInfo = Cast<AUTTeamInfo>(OptionalObject);
		Args.Add(TEXT("OptionalTeam"), TeamInfo ? TeamInfo->TeamName : FText::GetEmpty());
	}
}

bool UUTDomGameMessage::InterruptAnnouncement_Implementation(int32 Switch, const UObject* OptionalObject, TSubclassOf<UUTLocalMessage> OtherMessageClass, int32 OtherSwitch, const UObject* OtherOptionalObject) const
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

FName UUTDomGameMessage::GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject) const
{
	switch (Switch)
	{
		case 9: return TEXT("YouAreOnRedTeam"); break;
		case 10: return TEXT("YouAreOnBlueTeam"); break;
	}
	return NAME_None;
}

float UUTDomGameMessage::GetScaleInTime_Implementation(int32 MessageIndex) const
{
	return 0.15f;
}

float UUTDomGameMessage::GetScaleInSize_Implementation(int32 MessageIndex) const
{
	return 0.55f;
}

bool UUTDomGameMessage::UseLargeFont(int32 MessageIndex) const
{
	if ((MessageIndex == 0) || (MessageIndex == 1) || (MessageIndex == 2) || (MessageIndex == 3))
	{
		return false;
	}
	return (MessageIndex == 11) || (MessageIndex == 12) || (MessageIndex == 7) || (MessageIndex == 9) || (MessageIndex == 10);
}
