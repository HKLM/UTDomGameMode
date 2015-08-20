// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTLocalMessage.h"
#include "UTDoubleDomGameMessage.generated.h"

UCLASS()
class UUTDoubleDomGameMessage : public UUTLocalMessage
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

	virtual bool InterruptAnnouncement_Implementation(int32 Switch, const UObject* OptionalObject, TSubclassOf<UUTLocalMessage> OtherMessageClass, int32 OtherSwitch, const UObject* OtherOptionalObject) const override;
	virtual FText GetText(int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = NULL, class APlayerState* RelatedPlayerState_2 = NULL, class UObject* OptionalObject = NULL) const override;
	virtual void GetArgs(FFormatNamedArguments& Args, int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = NULL, class APlayerState* RelatedPlayerState_2 = NULL, class UObject* OptionalObject = NULL) const override;
	virtual FLinearColor GetMessageColor(int32 MessageIndex) const override;
	virtual FName GetTeamAnnouncement(int32 Switch, uint8 TeamIndex) const;
	virtual FName GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject) const override;
};
