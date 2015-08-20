// Created by Brian 'Snake' Alexander, 2015

#include "UnrealTournament.h"
#include "UTDomStats.h"
//#include "UTTeamGameMode.h"
//#include "UTDomGameMode.h"
//
//#include "StatNames.h"

AUTDomStats::AUTDomStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
/*
void AUTDomStats::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TempHandle;
	GetWorldTimerManager().SetTimer(TempHandle, this, &AUTDomStats::FlagHeldTimer, 1.0f, true);
}

void AUTDomStats::FlagHeldTimer()
{
	for (int32 FlagTeam = 0; FlagTeam < DomGameState->GameControlPoints.Num(); FlagTeam++)
	{
		AControlPoint* ControlPnt = DomGameState->GameControlPoints[FlagTeam];
		if (ControlPnt && ControlPnt->ControllingPawn)
		{
			// Look to see if the holder's ControlPnt is out (thus our holder is at minimum preventing the other team from scoring)
			//int32 HolderTeam = ControlPnt->Holder->GetTeamNum();
			//if (DomGameState->GameControlPoints.IsValidIndex(HolderTeam) && DomGameState->GameControlPoints[HolderTeam] && DomGameState->GameControlPoints[HolderTeam]->GetCarriedObjectHolder() != NULL)
			//{
			//	// score holder for keeping ControlPnt out, preventing capture.
			//	ControlPnt->Holder->Score += FlagHolderPointsPerSecond;
			//	ControlPnt->Holder->ModifyStatsValue(NAME_FlagHeldDeny, FlagHolderPointsPerSecond);
			//	ControlPnt->Holder->ModifyStatsValue(NAME_AttackerScore, FlagHolderPointsPerSecond);
			//	ControlPnt->Holder->ModifyStatsValue(NAME_FlagHeldDenyTime, 1.f);
			//}
			ControlPnt->ControllingPawn->ModifyStatsValue(NAME_DomPointHeldTime, 1.f);
			if (ControlPnt->ControllingTeam)
			{
				ControlPnt->ControllingTeam->ModifyStatsValue(NAME_TeamDomPointHeldTime, 1);
			}
		}
	}
}

float AUTDomStats::GetTotalHeldTime(AUTCarriedObject* GameObject)
{
	if (!GameObject)
	{
		return 0.f;
	}
	float TotalHeldTime = 0.f;
	for (int32 i = 0; i < GameObject->AssistTracking.Num(); i++)
	{
		AUTPlayerState* FlagRunner = GameObject->AssistTracking[i].Holder;
		if (FlagRunner != NULL)
		{
			TotalHeldTime += GameObject->GetHeldTime(FlagRunner);;
		}
	}
	return TotalHeldTime;
}
*/
/*
void AUTDomStats::ScoreTeam(uint8 ControlPointIndex, float TeamScoreAmount)
{
	if (!DomGameState)
	{
		return;
	}
		else if (Reason == FName("FlagCapture"))
		{
			FCTFScoringPlay NewScoringPlay;
			NewScoringPlay.Team = ScorerPS->Team;
			NewScoringPlay.ScoredBy = FSafePlayerName(ScorerPS);
			NewScoringPlay.TeamScores[0] = CTFGameState->Teams[0] ? CTFGameState->Teams[0]->Score : 0;
			NewScoringPlay.TeamScores[1] = CTFGameState->Teams[1] ? CTFGameState->Teams[1]->Score : 1;
			NewScoringPlay.TeamScores[ScorerPS->Team->TeamIndex] += 1;

			// TODO: need to handle no timelimit
			NewScoringPlay.RemainingTime = CTFGameState->bPlayingAdvantage ? 0.f : CTFGameState->RemainingTime;
			if (CTFGameState->IsMatchInOvertime())
			{
				NewScoringPlay.Period = 2;
			} else if (CTFGameState->bSecondHalf)
			{
				NewScoringPlay.Period = 1;
			}

			ScorerPS->FlagCaptures++;
			NewScoringPlay.ScoredByCaps = ScorerPS->FlagCaptures;
			int32 FlagPickupPoints = FlagFirstPickupPoints;
			float TotalHeldTime = GetTotalHeldTime(GameObject);
			for (int32 i = 0; i<GameObject->AssistTracking.Num(); i++)
			{
				AUTPlayerState* FlagRunner = GameObject->AssistTracking[i].Holder;
				if (FlagRunner != NULL)
				{
					float HeldTime = GameObject->GetHeldTime(FlagRunner);
					int32 Points = FlagPickupPoints;
					FlagPickupPoints = 0;
					if (HeldTime > 0.f && TotalHeldTime > 0.f)
					{
						Points = Points + int32(float(FlagRunScorePool * HeldTime / TotalHeldTime));
					}
					if (FlagRunner != ScorerPS)
					{
						FCTFAssist NewAssist;
						NewAssist.AssistName = FSafePlayerName(FlagRunner);
						NewAssist.bCarryAssist = true;
						NewScoringPlay.Assists.AddUnique(NewAssist);
						FlagRunner->ModifyStatsValue(NAME_CarryAssist, 1);
						FlagRunner->ModifyStatsValue(NAME_CarryAssistPoints, Points);
					} else
					{
						Points += FlagCapPoints;
						FlagRunner->ModifyStatsValue(NAME_FlagCapPoints, Points);
					}
					FlagRunner->AdjustScore(Points);
					FlagRunner->ModifyStatsValue(NAME_AttackerScore, Points);
					//UE_LOG(UT, Warning, TEXT("Flag assist (held) %s score %d"), *ScorerPS->PlayerName, Points);
				}
			}

}
*/
