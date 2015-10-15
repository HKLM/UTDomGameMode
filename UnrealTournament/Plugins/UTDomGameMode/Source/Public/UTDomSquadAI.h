// Created by Brian 'Snake' Alexander, 2015

#pragma once

#include "UnrealTournament.h"
#include "UTSquadAI.h"
#include "UTDomSquadAI.generated.h"

UCLASS(NotPlaceable)
class AUTDomSquadAI : public AUTSquadAI
{
	GENERATED_UCLASS_BODY()

	/** Array of DominationObjectives */
	UPROPERTY(BlueprintReadOnly, Category = DomSquad)
		TArray<AUTGameObjective*> GameControlPoints;

	/** Internal flag */
	UPROPERTY(Transient)
		bool bGotObjectives;

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
					B->WhatToDoNext();
				}
			}
		}
	}

	virtual void Initialize(AUTTeamInfo* InTeam, FName InOrders) override;
	virtual void BeginPlay() override;
	virtual bool CheckSquadObjectives(AUTBot* B) override;
	virtual bool IsNearEnemyBase(const FVector& TestLoc);
	virtual void NotifyObjectiveEvent(AActor* InObjective, AController* InstigatedBy, FName EventName) override;
	virtual void PickNewObjective(AActor* OldObjective, AUTPlayerState* InstigatedBy);

	/** Search for UTGameObjective in the map  */
	virtual void FindControlPoints();
};
