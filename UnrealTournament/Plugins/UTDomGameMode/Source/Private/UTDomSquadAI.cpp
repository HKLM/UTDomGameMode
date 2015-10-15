// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTSquadAI.h"
#include "UTDomSquadAI.h"
#include "ControlPoint.h"

FName NAME_Attack(TEXT("Attack"));
FName NAME_Defend(TEXT("Defend"));
FName NAME_Roam(TEXT("Roam"));

AUTDomSquadAI::AUTDomSquadAI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AUTDomSquadAI::Initialize(AUTTeamInfo* InTeam, FName InOrders)
{
	Super::Initialize(InTeam, InOrders);

	for (TActorIterator<AUTGameObjective> It(GetWorld()); It; ++It)
	{
		GameControlPoints.AddUnique(*It);
	}
	if (Orders == NAME_Attack)
	{
		if (GameControlPoints.IsValidIndex(0))
		{
			SetObjective(GameControlPoints[0]);
		}
	}
	else if (Orders == NAME_Defend)
	{
		if (GameControlPoints.IsValidIndex(1))
		{
			SetObjective(GameControlPoints[1]);
		} // Fallback to 1st control point
		else if (GameControlPoints.IsValidIndex(0))
		{
			SetObjective(GameControlPoints[0]);
		}
	}
	else if (Orders == NAME_Roam)
	{
		if (GameControlPoints.IsValidIndex(2))
		{
			SetObjective(GameControlPoints[2]);
		} // Fallback to 1st control point
		else if (GameControlPoints.IsValidIndex(0))
		{
			SetObjective(GameControlPoints[0]);
		}
	}
}

void AUTDomSquadAI::BeginPlay()
{
	NavData = GetUTNavData(GetWorld());
	Super::BeginPlay();
	FindControlPoints();
}

bool AUTDomSquadAI::CheckSquadObjectives(AUTBot* B)
{
	if (!bGotObjectives || GameObjective == NULL)
	{
		FindControlPoints();
	}

	if (B->NeedsWeapon() && B->FindInventoryGoal(0.0f))
	{
		B->GoalString = FString::Printf(TEXT("Get inventory %s"), *GetNameSafe(B->RouteCache.Last().Actor.Get()));
		B->SetMoveTarget(B->RouteCache[0]);
		B->StartWaitForMove();
		return true;
	}
	else if (GameObjective != NULL && GameObjective->GetTeamNum() != B->GetTeamNum() && B->LineOfSightTo(GameObjective))
	{
		return B->TryPathToward(GameObjective, false, "Goto ControlPoint lineofsight");
	}
	else if (GameObjective != NULL && GameObjective->GetTeamNum() != B->GetTeamNum())
	{
		return B->TryPathToward(GameObjective, true, "Goto ControlPoint quickly");
	}
	else if (GameObjective != NULL && GameObjective->GetTeamNum() == B->GetTeamNum())
	{
		return Super::CheckSquadObjectives(B);
	}
	else if ((GameObjective == NULL || GameObjective->GetTeamNum() != B->GetTeamNum()) &&
			 ((B->GetEnemy() == NULL && B->Personality.Aggressiveness <= 0.0f) || GetWorld()->TimeSeconds - B->LastRespawnTime < 10.0f * (1.0f - B->Personality.Aggressiveness)) &&
			 CheckSuperPickups(B, 5000))
	{
		return true;
	}
	else
	{
		return TryPathTowardObjective(B, GameObjective, false, "Attack objective");
	}
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

void AUTDomSquadAI::NotifyObjectiveEvent(AActor* InObjective, AController* InstigatedBy, FName EventName)
{
	AUTGameObjective* InGameObjective = Cast<AUTGameObjective>(InObjective);
	if (InstigatedBy != NULL && InGameObjective != NULL&& InGameObjective->GetCarriedObjectHolder() == InstigatedBy->PlayerState && Members.Contains(InstigatedBy))
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
	for (AController* C : Members)
	{
		AUTBot* B = Cast<AUTBot>(C);
		if (B != NULL)
		{
			if (B->GetMoveTarget().Actor != NULL && (B->GetMoveTarget().Actor == InGameObjective))
			{
				if (InGameObjective->GetTeamNum() == B->GetTeamNum())
				{
					PickNewObjective(InGameObjective, Cast<AUTPlayerState>(B->PlayerState));
					B->WhatToDoNext();
				}
				SetRetaskTimer(B);
			}
		}
	}
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
}
