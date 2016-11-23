// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTDomGameMode.h"
#include "UTSquadAI.h"
#include "UTDomSquadAI.h"
#include "UTDefensePoint.h"
#include "ControlPoint.h"

FName NAME_Attack(TEXT("Attack"));
FName NAME_Defend(TEXT("Defend"));
FName NAME_Roam(TEXT("Roam"));
FName NAME_Backup(TEXT("Backup"));

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
		if (!It->bHidden)
		{
			GameControlPoints.AddUnique(*It);
		}
	}
	int8 i = 0;

	if (Orders == NAME_Attack)
	{
		i = 0;
	}
	else if (Orders == NAME_Defend)
	{
		i = GameControlPoints.IsValidIndex(1) ? 1 : 0;
	}
	else if (Orders == NAME_Roam)
	{
		i = GameControlPoints.IsValidIndex(2) ? 2 : 0;
	}
	else
	{
		if (GameControlPoints.IsValidIndex(GameControlPoints.Num() - 1))
		{
			i = GameControlPoints.Num() - 1;
		}
		else if (GameControlPoints.IsValidIndex(0))
		{
			i = 0;
		}
	}
	// Extra validation to ensure game does not crash
	if (GameControlPoints.IsValidIndex(i))
	{
		SetObjective(GameControlPoints[i]);
		for (AController* C : Members)
		{
			AUTBot* B = Cast<AUTBot>(C);
			if (B != NULL)
			{
				B->WhatToDoNext();
			}
		}
	}
}

void AUTDomSquadAI::BeginPlay()
{
	Super::BeginPlay();
	FindControlPoints();
}

bool AUTDomSquadAI::CheckSquadObjectives(AUTBot* B)
{
	FString BotOrderString = "";
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

	AUTGameObjective* tmpObjective = NULL;

	int8 Lottery = FMath::RandRange(0, 10);
	bool bLottery = Lottery > 5 ? true : false;
	if (B->NeedsWeapon() && B->FindInventoryGoal(0.0f))
	{
		B->GoalString = FString::Printf(TEXT("Get inventory %s"), *GetNameSafe(B->RouteCache.Last().Actor.Get()));
		B->SetMoveTarget(B->RouteCache[0]);
		B->StartWaitForMove();
		return true;
	}
	else if (GameObjective != NULL && GameObjective->GetTeamNum() != B->GetTeamNum()
			 && B->LineOfSightTo(GameObjective)
			 && ((GameObjective->GetActorLocation() - B->GetPawn()->GetActorLocation()).Size() < 700.0f))
	{
		BotOrderString = FString::Printf(TEXT("%s-Goto ControlPoint lineofsight: %s"), *CurrentOrders.ToString(), *GetControlPointName(GameObjective));
		return B->TryPathToward(GameObjective, bLottery, true, BotOrderString);
	}
	else if (CurrentOrders == NAME_Attack || CurrentOrders == NAME_Roam)
	{
		if (GameObjective != NULL && GameObjective->GetTeamNum() != B->GetTeamNum())
		{
			BotOrderString = FString::Printf(TEXT("%s-Goto objective : %s"), *CurrentOrders.ToString(), *GetControlPointName(GameObjective));
			return B->TryPathToward(GameObjective, false, true, BotOrderString);
		}
		else if (CurrentOrders == NAME_Roam && (!B->LostContact(2.0f) || MustKeepEnemy(B->GetEnemy())))
		{
			B->GoalString = "Roam-Fight";
			return false;
		}
		else if (CheckSuperPickups(B, 5000))
		{
			return true;
		}
		else if (GameObjective != NULL && GameObjective->GetTeamNum() == B->GetTeamNum() && GetWorld()->GetTimeSeconds() - 10.0f > LastObjectiveChange)
		{
			tmpObjective = GetNewObjective(GameObjective, B);
			if (tmpObjective != NULL)
			{
				SetObjective(tmpObjective);
				BotOrderString = FString::Printf(TEXT("%s-Got New objective : %s"), *CurrentOrders.ToString(), *GetControlPointName(tmpObjective));
				return B->TryPathToward(tmpObjective, bLottery, true, BotOrderString);
			}
		}
		else if (!B->LostContact(2.0f) || MustKeepEnemy(B->GetEnemy()))
		{
			B->GoalString = "ATTACK-Fight";
			return false;
		}
	}
	else if (CurrentOrders == NAME_Defend || CurrentOrders == NAME_Backup)
	{
		if (CurrentOrders == NAME_Defend  && GameObjective->GetTeamNum() == B->GetTeamNum() && GetWorld()->GetTimeSeconds() - 120.0f > LastObjectiveChange)
		{
			tmpObjective = GetNewObjective(GameObjective, B);
			if (tmpObjective != NULL)
			{
				SetObjective(tmpObjective);
				BotOrderString = FString::Printf(TEXT("%s-Got New Objective : %s"), *CurrentOrders.ToString(), *GetControlPointName(tmpObjective));
				return B->TryPathToward(tmpObjective, false, true, BotOrderString);
			}
		}
		else if (GameObjective != NULL && GameObjective->GetTeamNum() != B->GetTeamNum())
		{
			BotOrderString = FString::Printf(TEXT("%s-Goto objective : %s"), *CurrentOrders.ToString(), *GetControlPointName(GameObjective));
			return B->TryPathToward(GameObjective, bLottery, true, BotOrderString);
		}
		else if (!B->LostContact(2.0f) || MustKeepEnemy(B->GetEnemy()))
		{
			B->GoalString = "DEFEND-Fight";
			return false;
		}
		else if (CurrentOrders == NAME_Backup && GameObjective != NULL && GameObjective->GetTeamNum() == B->GetTeamNum() && GetWorld()->GetTimeSeconds() - 30.0f > LastObjectiveChange)
		{
			tmpObjective = GetNewObjective(GameObjective, B);
			if (tmpObjective != NULL)
			{
				SetObjective(tmpObjective);
				BotOrderString = FString::Printf(TEXT("%s-Got New objective : %s"), *CurrentOrders.ToString(), *GetControlPointName(tmpObjective));
				return B->TryPathToward(tmpObjective, false, true, BotOrderString);
			}
		}
		else if (CheckSuperPickups(B, 5000))
		{
			return true;
		}
		else if (B->GetDefensePoint() != NULL)
		{
			BotOrderString = FString::Printf(TEXT("%s-Goto defense point : %s"), *CurrentOrders.ToString(), *B->GetDefensePoint()->GetName());
			return B->TryPathToward(B->GetDefensePoint(), true, true, BotOrderString);
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

AUTGameObjective* AUTDomSquadAI::GetNewObjective(AActor* OldObjective, AUTBot* B)
{
	LastObjectiveChange = GetWorld()->GetTimeSeconds();
	AUTGameObjective* BestObjective = NULL;
	AUTGameObjective* TheOldObjective = Cast<AUTGameObjective>(OldObjective);
	if (GameControlPoints.Num() < 2)
	{
		FindControlPoints();
	}
	// randomly pick random objective
	int8 randomObjective = FMath::RandRange(0, GameControlPoints.Num() - 1);
	int8 LotteryNumber = FMath::RandRange(0, GameControlPoints.Num() - 1);
	if (randomObjective == LotteryNumber && TheOldObjective != GameControlPoints[LotteryNumber])
	{
		return GameControlPoints[LotteryNumber];
	}

	if (B != NULL)
	{
		if (TheOldObjective != NULL)
		{
			for (uint8 i = 0; i < GameControlPoints.Num(); i++)
			{
				// Find a near by control point that our team does not control
				if ((TheOldObjective != GameControlPoints[i]
					&& GameControlPoints[i]->GetTeamNum() != B->GetTeamNum()
					&& B->LineOfSightTo(GameControlPoints[i]))
					|| ((GameControlPoints[i]->GetActorLocation() - B->GetPawn()->GetActorLocation()).Size() < 3000.0f))
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
					if (TheOldObjective != GameControlPoints[j]
						&& GameControlPoints[j]->GetTeamNum() != B->GetTeamNum())
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
				if (GameControlPoints[p]->GetTeamNum() != B->GetTeamNum()
					&& (B->LineOfSightTo(GameControlPoints[p])
					|| (GameControlPoints[p]->GetActorLocation() - B->GetPawn()->GetActorLocation()).Size() < 4000.0f))
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
	GameControlPoints.Empty();
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
	if (InstigatedBy != NULL && InGameObjective != NULL
		&& InGameObjective->GetCarriedObjectHolder() == InstigatedBy->PlayerState
		&& Members.Contains(InstigatedBy))
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
		if ((bOnlyNotControlled && (GameControlPoints[i]->GetTeamNum() != InstigatedBy->GetTeamNum()))
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
