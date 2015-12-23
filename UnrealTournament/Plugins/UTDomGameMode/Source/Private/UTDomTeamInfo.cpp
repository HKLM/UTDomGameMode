// Created by Brian 'Snake' Alexander, 2015

#include "UnrealTournament.h"
#include "UTTeamInfo.h"
#include "Net/UnrealNetwork.h"
#include "UTDomSquadAI.h"
#include "ControlPoint.h"
#include "UTDomTeamInfo.h"

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
	//conver to int and test if Score has changed. If not, dont waste network bandwidth for nothing.
	int32 i = FloatScore;
	if (Score != i)
	{
		Score = FloatScore;
	}
//#if NDEBUG || UE_BUILD_DEBUG
//	GEngine->AddOnScreenDebugMessage(-1, 1.f, TeamColor, FString::Printf(TEXT("Score:  %f, %d"), FloatScore, Score));
//#endif
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
