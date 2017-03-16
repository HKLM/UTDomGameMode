// Created by Brian 'Snake' Alexander, 2017
#pragma once

#include "UnrealTournament.h"
#include "UTVictoryMessage.h"
#include "UTDomVictoryMessage.generated.h"

UCLASS()
class UTDOMGAMEMODE_API UUTDomVictoryMessage : public UUTVictoryMessage
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
	FText GreenTeamName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
	FText GoldTeamName;

	virtual FName GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject, const class APlayerState* RelatedPlayerState_1, const class APlayerState* RelatedPlayerState_2) const override;
	virtual void GetEmphasisText(FText& PrefixText, FText& EmphasisText, FText& PostfixText, FLinearColor& EmphasisColor, int32 Switch, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const override;
};
