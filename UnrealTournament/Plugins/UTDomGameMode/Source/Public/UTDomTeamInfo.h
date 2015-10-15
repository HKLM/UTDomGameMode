// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTDomTeamInfo.generated.h"

UCLASS(NotPlaceable)
class AUTDomTeamInfo : public AUTTeamInfo
{
	GENERATED_UCLASS_BODY()

	/** team score as a float value */
	UPROPERTY(BlueprintReadWrite, Replicated, Category = DomTeam)
		float FloatScore;

	/** Adds ScorePoints value to the FloatScore and Score properties */
	UFUNCTION()
		virtual void SetFloatScore(float ScorePoints);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;
	virtual void AssignDefaultSquadFor(AController* C) override;
	virtual void NotifyObjectiveEvent(AActor* InObjective, AController* InstigatedBy, FName EventName) override;

};
