// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTLocalMessage.h"
#include "UTDomGameMessage.h"
#include "ControlPoint.h"

UUTDomGameMessage::UUTDomGameMessage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MessageArea = FName(TEXT("GameMessages"));
	ControlledByTeam = NSLOCTEXT("DOMGameMessage", "ControlledByTeam", "Control Point [{OptionalControlPoint}] now controlled by {OptionalTeam} Team!");

	bIsStatusAnnouncement = true;
	bIsPartiallyUnique = true;
}

FLinearColor UUTDomGameMessage::GetMessageColor_Implementation(int32 MessageIndex) const
{
	switch (MessageIndex)
	{
		case 0: return FLinearColor(3.0f, 0.0f, 0.05f, 0.8f); break;
		case 1: return FLinearColor(0.0f, 0.0f, 2.0f, 0.8f); break;
		case 2: return FLinearColor(0.0f, 1.0f, 0.0f, 0.8f); break;
		case 3: return FLinearColor(0.9f, 0.98f, 0.0f, 0.8f); break;
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
		Args.Add(TEXT("OptionalTeam"), CP->ControllingTeam ? CP->ControllingTeam->TeamName : FText::GetEmpty());
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
