// Created by Brian 'Snake' Alexander, 2015

#include "UnrealTournament.h"
#include "UTDomTeamInfo.h"
#include "UTTeamInfo.h"
#include "UnrealNetwork.h"
#include "UTDomSquadAI.h"
#include "DominationObjective.h"

AUTDomTeamInfo::AUTDomTeamInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultOrders.Add(FName(TEXT("Roam")));
}

void AUTDomTeamInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUTDomTeamInfo, FloatScore);
}

void AUTDomTeamInfo::SetFloatScore(float ScorePoints)
{
	FloatScore += ScorePoints;
	Score = FloatScore;
#if NDEBUG || UE_BUILD_DEBUG
	GEngine->AddOnScreenDebugMessage(-1, 1.f, TeamColor, FString::Printf(TEXT("FloatScore:  %f"), FloatScore));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, TeamColor, FString::Printf(TEXT("Score:  %d"), Score));
#endif
}

void AUTDomTeamInfo::AssignDefaultSquadFor(AController* C)
{
	if (Cast<AUTBot>(C) != NULL)
	{
		if (DefaultOrders.Num() > 0)
		{
			DefaultOrderIndex = (DefaultOrderIndex + 1) % DefaultOrders.Num();
			AssignToSquad(C, DefaultOrders[DefaultOrderIndex]);
		}
		else
		{
			AssignToSquad(C, NAME_None);
		}
	}
	else
	{
		// TODO: playercontroller
	}
}

void AUTDomTeamInfo::NotifyObjectiveEvent(AActor* InObjective, AController* InstigatedBy, FName EventName)
{
	if (Cast<ADominationObjective>(InObjective) == NULL)
	{
		return;
	}

	ADominationObjective* d = Cast<ADominationObjective>(InObjective);

	for (AUTSquadAI* Squad : Squads)
	{
		// if this squads objective is no longer held by our team, let them know
		if (Squad != NULL && !Squad->bPendingKillPending && Squad->Team->GetTeamNum() != d->GetControlPoint()->GetTeamNum())
		{
			if (Squad->GetGameObjective() == d)
			{
				Squad->NotifyObjectiveEvent(InObjective, InstigatedBy, EventName);
			}
		}
	}
}
