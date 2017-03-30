// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTLocalMessage.h"
#include "UTDoubleDomGameMessage.h"
#include "UTAnnouncer.h"
#include "xDomPoint.h"

UUTDoubleDomGameMessage::UUTDoubleDomGameMessage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MessageArea = FName(TEXT("Announcements"));
	MessageSlot = FName(TEXT("GameMessages"));

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

	static ConstructorHelpers::FObjectFinder<USoundBase> NewRoundInSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerStatus/A_AnnouncerF_NewRoundIn.A_AnnouncerF_NewRoundIn'"));
	NewRoundInSound = NewRoundInSoundFinder.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> AvertedSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerStatus/A_AnnouncerF_Averted.A_AnnouncerF_Averted'"));
	AvertedSound = AvertedSoundFinder.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> RedTeamScoresSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerStatus/A_AnnouncerF_RedTeamScores.A_AnnouncerF_RedTeamScores'"));
	RedTeamScoresSound = RedTeamScoresSoundFinder.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> BlueTeamScoresSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerStatus/A_AnnouncerF_BlueTeamScores.A_AnnouncerF_BlueTeamScores'"));
	BlueTeamScoresSound = BlueTeamScoresSoundFinder.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> RedTeamDominatingSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerStatus/A_AnnouncerF_RedTeamDominating.A_AnnouncerF_RedTeamDominating'"));
	RedTeamDominatingSound = RedTeamDominatingSoundFinder.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> BlueTeamDominatingSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerStatus/A_AnnouncerF_BlueTeamDominating.A_AnnouncerF_BlueTeamDominating'"));
	BlueTeamDominatingSound = BlueTeamDominatingSoundFinder.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> LastSecondSaveSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerStatus/A_AnnouncerF_LastSecondSave.A_AnnouncerF_LastSecondSave'"));
	LastSecondSaveSound = LastSecondSaveSoundFinder.Object;
	//static ConstructorHelpers::FObjectFinder<USoundBase> NextPointWinsSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerStatus/A_AnnouncerF_NextPointWins.A_AnnouncerF_NextPointWins'"));
	//NextPointWinsSound = NextPointWinsSoundFinder.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> BlueIncreasesLeadSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerStatus/A_AnnouncerF_BlueIncreasesLead.A_AnnouncerF_BlueIncreasesLead'"));
	BlueIncreasesLeadSound = BlueIncreasesLeadSoundFinder.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> RedIncreasesLeadSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerStatus/A_AnnouncerF_RedIncreasesLead.A_AnnouncerF_RedIncreasesLead'"));
	RedIncreasesLeadSound = RedIncreasesLeadSoundFinder.Object;
	//static ConstructorHelpers::FObjectFinder<USoundBase> LastSecondSaveSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerReward/A_Announcer_Last_Second_Save.A_Announcer_Last_Second_Save'"));
	static ConstructorHelpers::FObjectFinder<USoundBase> NarrowlyAvertedSoundFinder(TEXT("SoundWave'/Game/RestrictedAssets/Audio/AnnouncerReward/A_Announcer_NarrowlyAverted.A_Announcer_NarrowlyAverted'"));
	NarrowlyAvertedSound = NarrowlyAvertedSoundFinder.Object;
}

void UUTDoubleDomGameMessage::ClientReceive(const FClientReceiveData& ClientData) const
{
	Super::ClientReceive(ClientData);
	AUTPlayerController* PC = Cast<AUTPlayerController>(ClientData.LocalPC);
	if (PC)
	{
		if (ClientData.MessageIndex == 15)
		{
			PC->UTClientPlaySound(AvertedSound);
		}
		else if (ClientData.MessageIndex == 14)
		{
			PC->UTClientPlaySound(NewRoundInSound);
		}
	}
}

FLinearColor UUTDoubleDomGameMessage::GetMessageColor_Implementation(int32 MessageIndex) const
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
		AxDomPoint* CP = Cast<AxDomPoint>(OptionalObject);
		Args.Add(TEXT("OptionalControlPoint"), CP ? FText::FromString(CP->PointName) : FText::GetEmpty());
		AMultiTeamTeamInfo* DTI = Cast<AMultiTeamTeamInfo>(CP->ControllingTeam);
		if (DTI != NULL)
		Args.Add(TEXT("OptionalTeam"), DTI ? DTI->TeamName : FText::GetEmpty());
	}
	else
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
		if (OptionalObject == OtherOptionalObject)
		{
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
		case 13: 
			if (TeamNum == 0)
			{
				return TEXT("RedDominating"); break;
			}
			else if (TeamNum == 1)
			{
				return TEXT("BlueDominating"); break;
			}
		case 14: return TEXT("NewRoundIn"); break;
		case 15: return TEXT("Averted"); break;
	}
	return NAME_None;
}

FName UUTDoubleDomGameMessage::GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject, const class APlayerState* RelatedPlayerState_1, const class APlayerState* RelatedPlayerState_2) const
{
	switch (Switch)
	{
		case 9: return TEXT("YouAreOnRedTeam"); break;
		case 10: return TEXT("YouAreOnBlueTeam"); break;
	}
	return NAME_None;
}

float UUTDoubleDomGameMessage::GetScaleInTime_Implementation(int32 MessageIndex) const
{
	return 0.15f;
}

float UUTDoubleDomGameMessage::GetScaleInSize_Implementation(int32 MessageIndex) const
{
	return 0.55f;
}

int32 UUTDoubleDomGameMessage::GetFontSizeIndex(int32 MessageIndex, bool bTargetsLocalPlayer) const
{
	if ((MessageIndex == 0) || (MessageIndex == 1) || (MessageIndex == 2) || (MessageIndex == 3))
	{
		return 2;
	}
	else
	{
	//if ((MessageIndex == 11) || (MessageIndex == 12) || (MessageIndex == 7) || (MessageIndex == 9) || (MessageIndex == 10))
		return 1;
	}
}
