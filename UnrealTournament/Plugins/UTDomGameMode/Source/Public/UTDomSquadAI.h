// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTSquadAI.h"
#include "ControlPoint.h"
#include "UTDomSquadAI.generated.h"

USTRUCT()
struct FDomAITargets
{
	GENERATED_USTRUCT_BODY()

	AUTGameObjective* Target;
	float Distance;
};

UCLASS(NotPlaceable)
class UTDOMGAMEMODE_API AUTDomSquadAI : public AUTSquadAI
{
	GENERATED_UCLASS_BODY()

	/** Array of DominationObjectives */
	TArray<AUTGameObjective*> GameControlPoints;

	/** Internal flag */
	UPROPERTY(Transient)
		bool bGotObjectives;

	virtual void Initialize(AUTTeamInfo* InTeam, FName InOrders) override;
	virtual void BeginPlay() override;
	virtual bool CheckSquadObjectives(AUTBot* B) override;
	virtual bool IsNearEnemyBase(const FVector& TestLoc);

	virtual void SetObjective(AActor* InObjective) override
	{
		if (InObjective != Objective)
		{
			Objective = InObjective;
			GameObjective = Cast<AUTGameObjective>(InObjective);
			SquadRoutes.Empty();
			CurrentSquadRouteIndex = INDEX_NONE;
			for (AController* C : Members)
			{
				AUTBot* B = Cast<AUTBot>(C);
				if (B != NULL)
				{
					B->UsingSquadRouteIndex = INDEX_NONE;
					B->bDisableSquadRoutes = false;
					B->SquadRouteGoal.Clear();
					//B->WhatToDoNext();
				}
			}
		}
	}

	virtual AUTGameObjective* GetNewObjective(AActor* OldObjective, AUTBot* B);

	virtual AUTGameObjective* GetNearestObjective(AUTBot* InstigatedBy, bool bOnlyNotControlled);

	/** Search for UTGameObjective in the map  */
	virtual void FindControlPoints();
	virtual void NotifyObjectiveEvent(AActor* InObjective, AController* InstigatedBy, FName EventName) override;

	virtual FString GetControlPointName(AUTGameObjective* ObjectiveToCheck) const
	{
		AControlPoint* CP = Cast<AControlPoint>(ObjectiveToCheck);
		return (CP != nullptr ? *CP->GetPointName() : *ObjectiveToCheck->GetHumanReadableName());
	}

protected:
	/** Internal counter to prevent excessive Objectie changes */
	float LastObjectiveChange;

};
