// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTCarriedObject.h"
#include "Net/UnrealNetwork.h"
#include "BaseControlPoint.h"

ABaseControlPoint::ABaseControlPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ABaseControlPoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseControlPoint, ControllingPawn);
	DOREPLIFETIME(ABaseControlPoint, ControllingTeam);
	DOREPLIFETIME(ABaseControlPoint, ControllingTeamNum);
	DOREPLIFETIME_CONDITION(ABaseControlPoint, PointName, COND_None);
}

FString ABaseControlPoint::GetPointName()
{
	return (PointName.IsEmpty()) ? TEXT("ControlPoint") : PointName;
}

AUTPlayerState* ABaseControlPoint::GetControlPointHolder()
{
	return ControllingPawn;
}

int32 ABaseControlPoint::GetControllingTeamNum()
{
	if (ControllingTeamNum == 255)
	{
		return 4;
	}
	else
	{
		return ControllingTeamNum;
	}
}
