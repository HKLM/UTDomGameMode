// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTDomTeamInfo.generated.h"

UCLASS(NotPlaceable)
class UTDOMGAMEMODE_API AUTDomTeamInfo : public AUTTeamInfo
{
	GENERATED_UCLASS_BODY()

protected:
	/** team score as a float value */
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Replicated, Category = DomTeam)
	float FloatScore;

public:
	/** Adds ScorePoints value to the FloatScore and Score properties */
	UFUNCTION()
	virtual void SetFloatScore(float ScorePoints);

	/** 
	 * Returns the teams FloatScore value 
	 * @return	The current value of FloatScore
	 */
	UFUNCTION(BlueprintCallable, Category = DomTeam)
	virtual float GetFloatScore() { return FloatScore; }

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;
	virtual bool AssignToSquad(AController* C, FName Orders, AController* Leader = NULL) override;
	virtual void AssignDefaultSquadFor(AController* C) override;

	virtual int32 GetNextOrderIndex();
};
