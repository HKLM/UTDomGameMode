// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTSquadAI.h"
#include "UTDomSquadAI.generated.h"

USTRUCT()
struct FDomAITargets
{
	GENERATED_USTRUCT_BODY()

	AUTGameObjective* Target;
	float Distance;
};

UCLASS(NotPlaceable)
class AUTDomSquadAI : public AUTSquadAI
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
	virtual void PickNewObjective(AActor* OldObjective, AUTPlayerState* InstigatedBy);

	virtual AActor* GetNewObjective(AActor* OldObjective, AUTPlayerState* InstigatedBy);

	virtual AUTGameObjective* GetNearestObjective(AUTBot* InstigatedBy, bool bOnlyNotControlled);

	/** Search for UTGameObjective in the map  */
	virtual void FindControlPoints();
	virtual void NotifyObjectiveEvent(AActor* InObjective, AController* InstigatedBy, FName EventName) override;

	FString GetControlPointName(AUTGameObjective* ObjectiveToCheck) const;
};
