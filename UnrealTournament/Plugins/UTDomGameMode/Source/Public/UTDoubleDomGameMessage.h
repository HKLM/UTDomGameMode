// Created by Brian 'Snake' Alexander, 2017
#pragma once

#include "UnrealTournament.h"
#include "UTLocalMessage.h"
#include "UTDoubleDomGameMessage.generated.h"

UCLASS(NotPlaceable)
class UTDOMGAMEMODE_API UUTDoubleDomGameMessage : public UUTLocalMessage
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		FText ControlledByTeam;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		FText ControlsBothPointsString;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		FText NewRoundIn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		FText AvertedText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Message)
		TArray<FText> DDTimerText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		USoundBase* NewRoundInSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		USoundBase* AvertedSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		USoundBase* NarrowlyAvertedSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		USoundBase* LastSecondSaveSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		USoundBase* RedTeamScoresSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		USoundBase* BlueTeamScoresSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		USoundBase* RedTeamDominatingSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		USoundBase* BlueTeamDominatingSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		USoundBase* RedIncreasesLeadSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
		USoundBase* BlueIncreasesLeadSound;


	virtual void ClientReceive(const FClientReceiveData& ClientData) const override;
	virtual bool InterruptAnnouncement_Implementation(int32 Switch, const UObject* OptionalObject, TSubclassOf<UUTLocalMessage> OtherMessageClass, int32 OtherSwitch, const UObject* OtherOptionalObject) const override;
	virtual FName GetTeamAnnouncement(int32 Switch, uint8 TeamNum) const;
	virtual FText GetText(int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = NULL, class APlayerState* RelatedPlayerState_2 = NULL, class UObject* OptionalObject = NULL) const override;
	virtual void GetArgs(FFormatNamedArguments& Args, int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = NULL, class APlayerState* RelatedPlayerState_2 = NULL, class UObject* OptionalObject = NULL) const override;
	virtual FLinearColor GetMessageColor_Implementation(int32 MessageIndex) const override;
	virtual float GetScaleInTime_Implementation(int32 MessageIndex) const override;
	virtual float GetScaleInSize_Implementation(int32 MessageIndex) const override;
	virtual FName GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject, const class APlayerState* RelatedPlayerState_1, const class APlayerState* RelatedPlayerState_2) const override;
	virtual int32 GetFontSizeIndex(int32 MessageIndex, bool bTargetsLocalPlayer) const override;

};
