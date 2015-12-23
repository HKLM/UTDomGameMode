// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTSquadAI.h"
#include "UTDomSquadAI.h"
#include "UTDefensePoint.h"
#include "ControlPoint.h"

FName NAME_Attack(TEXT("Attack"));
FName NAME_Defend(TEXT("Defend"));
FName NAME_Roam(TEXT("Roam"));

AUTDomSquadAI::AUTDomSquadAI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bGotObjectives = false;
}

void AUTDomSquadAI::Initialize(AUTTeamInfo* InTeam, FName InOrders)
{
	Super::Initialize(InTeam, InOrders);

	for (TActorIterator<AUTGameObjective> It(GetWorld()); It; ++It)
	{
		GameControlPoints.AddUnique(*It);
	}
	int8 i = FMath::RandRange(0, GameControlPoints.Num());
	if (!GameControlPoints.IsValidIndex(i))
	{
		i = 0;
	}

	if (Orders == NAME_Attack)
	{
		SetObjective(GameControlPoints[i]);
	}
	else if (Orders == NAME_Defend)
	{
		SetObjective(GameControlPoints[i]);
	}
	else if (Orders == NAME_Roam)
	{
		SetObjective(GameControlPoints[i]);
	}
}

void AUTDomSquadAI::BeginPlay()
{
	Super::BeginPlay();
	FindControlPoints();
}

bool AUTDomSquadAI::CheckSquadObjectives(AUTBot* B)
{
	FName CurrentOrders = GetCurrentOrders(B);
	if (CurrentOrders == NAME_Defend)
	{
		SetDefensePointFor(B);
	}
	else
	{
		B->SetDefensePoint(NULL);
	}

	if (!bGotObjectives || GameObjective == NULL)
	{
		FindControlPoints();
	}

	int8 Lottery = FMath::RandRange(0, 10);
	bool bLottery = Lottery > 5 ? true : false;

	if (B->NeedsWeapon() && B->FindInventoryGoal(0.0f))
	{
		B->GoalString = FString::Printf(TEXT("Get inventory %s"), *GetNameSafe(B->RouteCache.Last().Actor.Get()));
		B->SetMoveTarget(B->RouteCache[0]);
		B->StartWaitForMove();
		return true;
	}
	else if (GameObjective->GetTeamNum() != B->GetTeamNum() && B->LineOfSightTo(GameObjective))
	{
		return B->TryPathToward(GameObjective, bLottery, "Goto ControlPoint lineofsight");
	}
	else if (GameObjective->GetTeamNum() != B->GetTeamNum() && (GameObjective->GetActorLocation() - B->GetPawn()->GetActorLocation()).Size() < 3000.0f)
	{
		return B->TryPathToward(GameObjective, false, "Goto ControlPoint");
	}
	else if (GameObjective->GetTeamNum() == B->GetTeamNum())
	{
		if (B->GetEnemy() != NULL)
		{
			if (!B->LostContact(2.0f) || MustKeepEnemy(B->GetEnemy()))
			{
				B->GoalString = "Fight attacker";
				return false;
			}
			else if (CheckSuperPickups(B, 5000))
			{
				return true;
			}
			else if (CurrentOrders == NAME_Defend  && B->GetDefensePoint() != NULL)
			{
				return B->TryPathToward(B->GetDefensePoint(), true, "Go to defense point");
			}
			else if (CurrentOrders == NAME_Roam)
			{
				if (B->GetEnemy() == NULL || !MustKeepEnemy(B->GetEnemy()))
				{
					return B->TryPathToward(GameObjective, bLottery, "Goto ControlPoint lineofsight");
				}
				else
				{
					B->GoalString = "Fight attacker";
					return false;
				}
			}
		}
		if (CurrentOrders == NAME_Attack || CurrentOrders == NAME_Roam)
		{
			return B->TryPathToward(GetNearestObjective(B, true), false, "Going to nearest objective");
			//if (B->GetEnemy() != NULL && !B->LostContact(2.0f))
			//{
			//	B->GoalString = "Fightn enemy ControlPoint Guard";
			//	return false;
			//}
			//else if (B->GetEnemy() == NULL || !MustKeepEnemy(B->GetEnemy()))
			//{
			//	if (bLottery)
			//	{
			//		return TryPathTowardObjective(B, GetNewObjective(GameObjective, Cast<AUTPlayerState>(B->PlayerState)), bLottery, "Got New controloPoint");
			//	}
			//	else
			//	{
			//		return TryPathTowardObjective(B, GameObjective, true, "Going to controloPoint");
			//	}
			//}
		}
		else if (CurrentOrders == NAME_Defend && B->GetDefensePoint() != NULL)
		{
			return B->TryPathToward(B->GetDefensePoint(), bLottery, "Go to defense point");
		}
	}
	else if ((GameObjective->GetTeamNum() != B->GetTeamNum()) &&
			 ((B->GetEnemy() == NULL && B->Personality.Aggressiveness <= 0.0f) || GetWorld()->TimeSeconds - B->LastRespawnTime < 10.0f * (1.0f - B->Personality.Aggressiveness)) &&
			 CheckSuperPickups(B, 5000))
	{
		return true;
	}
	else if (Cast<APlayerController>(Leader) != NULL && Leader->GetPawn() != NULL)
	{
		if (B->NeedsWeapon() && B->FindInventoryGoal(0.0f))
		{
			B->GoalString = FString::Printf(TEXT("Get inventory %s"), *GetNameSafe(B->RouteCache.Last().Actor.Get()));
			B->SetMoveTarget(B->RouteCache[0]);
			B->StartWaitForMove();
			return true;
		}
		else if (B->GetEnemy() != NULL && !B->LostContact(2.0f))
		{
			// fight!
			return false;
		}
		else
		{
			return B->TryPathToward(Leader->GetPawn(), true, "Find leader");
		}
	}
	return Super::CheckSquadObjectives(B);
}

bool AUTDomSquadAI::IsNearEnemyBase(const FVector& TestLoc)
{
	for (uint8 i = 0; i < GameControlPoints.Num(); i++)
	{
		if ((TestLoc - GameControlPoints[i]->GetActorLocation()).Size() < 2500.0f)
		{
			return true;
		}
	}
	return false;
}

void AUTDomSquadAI::PickNewObjective(AActor* OldObjective, AUTPlayerState* InstigatedBy)
{
	AUTGameObjective* BestObjective = NULL;
	AUTGameObjective* TheOldObjective = Cast<AUTGameObjective>(OldObjective);
	if (GameControlPoints.Num() < 2)
	{
		FindControlPoints();
	}
	AUTBot* B = Cast<AUTBot>(InstigatedBy->GetOwner());
	if (B != NULL)
	{
		if (TheOldObjective != NULL)
		{
			for (uint8 i = 0; i < GameControlPoints.Num(); i++)
			{
				// Find a near by control point that our team does not control
				if ((TheOldObjective != GameControlPoints[i] && GameControlPoints[i]->GetTeamNum() != B->GetTeamNum() && B->LineOfSightTo(GameControlPoints[i])) || ((GameControlPoints[i]->GetActorLocation() - B->GetPawn()->GetActorLocation()).Size() < 3000.0f))
				{
					BestObjective = GameControlPoints[i];
					break;
				}
			}
			if (BestObjective == NULL)
			{
				// Find just a control point that our team does not control
				for (uint8 j = 0; j < GameControlPoints.Num(); j++)
				{
					if (TheOldObjective != GameControlPoints[j] && GameControlPoints[j]->GetTeamNum() != B->GetTeamNum())
					{
						BestObjective = GameControlPoints[j];
						break;
					}
				}
				// Find a control point that is a differant then the original one
				if (BestObjective == NULL)
				{
					for (uint8 l = 0; l < GameControlPoints.Num(); l++)
					{
						if (TheOldObjective != GameControlPoints[l])
						{
							BestObjective = GameControlPoints[l];
							break;
						}
					}
				}
			}
		}
		if (BestObjective == NULL)
		{
			for (uint8 p = 0; p < GameControlPoints.Num(); p++)
			{
				// Find a near by control point that our team does not control
				if (GameControlPoints[p]->GetTeamNum() != B->GetTeamNum() && (B->LineOfSightTo(GameControlPoints[p]) || (GameControlPoints[p]->GetActorLocation() - B->GetPawn()->GetActorLocation()).Size() < 4000.0f))
				{
					BestObjective = GameControlPoints[p];
					break;
				}
			}
		}
		// Last resort, pick anything
		if (BestObjective == NULL)
		{
			for (uint8 k = 0; k < GameControlPoints.Num(); k++)
			{
				BestObjective = GameControlPoints[k];
				break;
			}
		}
		if (BestObjective != NULL)
		{
			bGotObjectives = true;
			B->GoalString = "Retasked Objectives";
			SetObjective(BestObjective);
		}
	}
}

AActor* AUTDomSquadAI::GetNewObjective(AActor* OldObjective, AUTPlayerState* InstigatedBy)
{
	AUTGameObjective* BestObjective = NULL;
	AUTGameObjective* TheOldObjective = Cast<AUTGameObjective>(OldObjective);
	if (GameControlPoints.Num() < 2)
	{
		FindControlPoints();
	}
	// randomly pick random objective
	int8 randomObjective = FMath::RandRange(0, GameControlPoints.Num() - 1);
	int8 LotteryNumber = FMath::RandRange(0, GameControlPoints.Num() - 1);
	if (randomObjective == LotteryNumber)
	{
		return GameControlPoints[LotteryNumber];
	}

	AUTBot* B = Cast<AUTBot>(InstigatedBy->GetOwner());
	if (B != NULL)
	{
		if (TheOldObjective != NULL)
		{
			for (uint8 i = 0; i < GameControlPoints.Num(); i++)
			{
				// Find a near by control point that our team does not control
				if ((TheOldObjective != GameControlPoints[i] && GameControlPoints[i]->GetTeamNum() != B->GetTeamNum() && B->LineOfSightTo(GameControlPoints[i])) || ((GameControlPoints[i]->GetActorLocation() - B->GetPawn()->GetActorLocation()).Size() < 3000.0f))
				{
					BestObjective = GameControlPoints[i];
					break;
				}
			}
			if (BestObjective == NULL)
			{
				// Find just a control point that our team does not control
				for (uint8 j = 0; j < GameControlPoints.Num(); j++)
				{
					if (TheOldObjective != GameControlPoints[j] && GameControlPoints[j]->GetTeamNum() != B->GetTeamNum())
					{
						BestObjective = GameControlPoints[j];
						break;
					}
				}
				// Find a control point that is a differant then the original one
				if (BestObjective == NULL)
				{
					for (uint8 l = 0; l < GameControlPoints.Num(); l++)
					{
						if (TheOldObjective != GameControlPoints[l])
						{
							BestObjective = GameControlPoints[l];
							break;
						}
					}
				}
			}
		}
		if (BestObjective == NULL)
		{
			for (uint8 p = 0; p < GameControlPoints.Num(); p++)
			{
				// Find a near by control point that our team does not control
				if (GameControlPoints[p]->GetTeamNum() != B->GetTeamNum() && (B->LineOfSightTo(GameControlPoints[p]) || (GameControlPoints[p]->GetActorLocation() - B->GetPawn()->GetActorLocation()).Size() < 4000.0f))
				{
					BestObjective = GameControlPoints[p];
					break;
				}
			}
		}
		// Last resort, pick anything
		if (BestObjective == NULL)
		{
			for (uint8 k = 0; k < GameControlPoints.Num(); k++)
			{
				BestObjective = GameControlPoints[k];
				break;
			}
		}
	}
	return BestObjective;
}

void AUTDomSquadAI::FindControlPoints()
{
	//GameControlPoints.Empty();
	for (TActorIterator<AUTGameObjective> It(GetWorld()); It; ++It)
	{
		if (!It->bHidden)
		{
			GameControlPoints.AddUnique(*It);
		}
	}
	bGotObjectives = true;
}

void AUTDomSquadAI::NotifyObjectiveEvent(AActor* InObjective, AController* InstigatedBy, FName EventName)
{
	AUTGameObjective* InGameObjective = Cast<AUTGameObjective>(InObjective);
	if (InstigatedBy != NULL && InGameObjective != NULL && InGameObjective->GetCarriedObjectHolder() == InstigatedBy->PlayerState && Members.Contains(InstigatedBy))
	{
		// re-enable alternate paths for flag carrier so it can consider them for planning its escape
		AUTBot* B = Cast<AUTBot>(InstigatedBy);
		if (B != NULL)
		{
			B->bDisableSquadRoutes = false;
			B->SquadRouteGoal.Clear();
			B->UsingSquadRouteIndex = INDEX_NONE;
		}
		SetLeader(InstigatedBy);
	}

	Super::NotifyObjectiveEvent(InObjective, InstigatedBy, EventName);
}

AUTGameObjective* AUTDomSquadAI::GetNearestObjective(AUTBot* InstigatedBy, bool bOnlyNotControlled)
{
	AUTGameObjective* BestObjective = NULL;
	float bestDistance = 100000.0f;

	FDomAITargets f;
	TArray<FDomAITargets> CP;

	for (uint8 i = 0; i < GameControlPoints.Num(); i++)
	{
		if (bOnlyNotControlled && GameControlPoints[i]->GetTeamNum() != InstigatedBy->GetTeamNum()
			|| !bOnlyNotControlled)
		{
			f.Target = GameControlPoints[i];
			f.Distance = (GameControlPoints[i]->GetActorLocation() - InstigatedBy->GetPawn()->GetActorLocation()).Size();
			CP.Add(f);
		}
	}
	if (!CP.IsValidIndex(0))
	{
		// Last time nothing was found, so lets add then all without restriction
		for (uint8 i = 0; i < GameControlPoints.Num(); i++)
		{
			f.Target = GameControlPoints[i];
			f.Distance = (GameControlPoints[i]->GetActorLocation() - InstigatedBy->GetPawn()->GetActorLocation()).Size();
			CP.Add(f);
		}
	}
	if (CP.IsValidIndex(0))
	{
		for (uint8 i = 0; i < CP.Num(); i++)
		{
			if (i == 0)
			{
				bestDistance = CP[i].Distance;
				BestObjective = CP[i].Target;
			}
			else if (CP[i].Distance < bestDistance)
			{
				bestDistance = CP[i].Distance;
				BestObjective = CP[i].Target;
			}
		}
	}
	return BestObjective;
}
