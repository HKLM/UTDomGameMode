// Created by Brian 'Snake' Alexander, 2015

#include "UnrealTournament.h"
#include "UTTeamInfo.h"
#include "Net/UnrealNetwork.h"
#include "UTDomSquadAI.h"
#include "UTDomGameMode.h"
#include "UTDomTeamInfo.h"

AUTDomTeamInfo::AUTDomTeamInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultOrders.Add(FName(TEXT("Roam")));
	DefaultOrders.Add(FName(TEXT("Backup")));
}

void AUTDomTeamInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUTDomTeamInfo, FloatScore);
}

void AUTDomTeamInfo::SetFloatScore(float ScorePoints)
{
	if (Role == ROLE_Authority)
	{
		FloatScore += ScorePoints;
		//conver to int and test if Score has changed. If not, dont waste network bandwidth for nothing.
		int32 i = FloatScore;
		if (Score != i)
		{
			Score = FloatScore;
		}
	}
}

bool AUTDomTeamInfo::AssignToSquad(AController* C, FName Orders, AController* Leader)
{
	AUTDomSquadAI* NewSquad = nullptr;
	for (int32 i = 0; i < Squads.Num(); i++)
	{
		if (Squads[i] == nullptr || Squads[i]->IsPendingKillPending())
		{
			Squads.RemoveAt(i--);
		}
		else if (Squads[i]->Orders == Orders && (Leader == nullptr || Squads[i]->GetLeader() == Leader) && (Leader != nullptr || Squads[i]->GetSize() < GetWorld()->GetAuthGameMode<AUTDomGameMode>()->MaxSquadSize))
		{
			NewSquad = Cast<AUTDomSquadAI>(Squads[i]);
			break;
		}
	}
	if (NewSquad == nullptr && (Leader == nullptr || Leader == C))
	{
		NewSquad = GetWorld()->SpawnActor<AUTDomSquadAI>(GetWorld()->GetAuthGameMode<AUTDomGameMode>()->SquadType);
		NewSquad->Initialize(this, Orders);
		Squads.Add(NewSquad);
	}
	if (NewSquad == nullptr)
	{
		return false;
	}
	else
	{
		// assign squad
		AUTBot* B = Cast<AUTBot>(C);
		if (B != nullptr)
		{
			B->SetSquad(NewSquad);
		}
		else
		{
			// TODO: playercontroller
		}
		return true;
	}
}

void AUTDomTeamInfo::AssignDefaultSquadFor(AController* C)
{
	if (Cast<AUTBot>(C) != nullptr)
	{
		if (DefaultOrders.Num() > 0)
		{
			AssignToSquad(C, DefaultOrders[GetNextOrderIndex()]);
		}
		else
		{
			AssignToSquad(C, FName(TEXT("Attack")));
		}
	}
}

int32 AUTDomTeamInfo::GetNextOrderIndex()
{
	DefaultOrderIndex++;
	if (DefaultOrderIndex >= DefaultOrders.Num())
	{
		DefaultOrderIndex = 0;
	}

	return DefaultOrderIndex;
}
