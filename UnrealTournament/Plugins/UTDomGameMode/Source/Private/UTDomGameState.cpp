// Created by Brian 'Snake' Alexander, 2015

#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "Net/UnrealNetwork.h"
#include "UTDomGameMode.h"
#include "StatNames.h"
#include "UTGameState.h"
#include "UTDomPlayerState.h"
#include "UTDomStat.h"
#include "UTDomGameState.h"

AUTDomGameState::AUTDomGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameScoreStats.Add(NAME_RegularKillPoints);
	GameScoreStats.Add(NAME_ControlPointCaps);
	GameScoreStats.Add(NAME_ControlPointHeldTime);
	GameScoreStats.Add(NAME_ControlPointHeldPoints);

	TeamStats.Add(NAME_TeamControlPointHeldTime);
}

void AUTDomGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUTDomGameState, GameControlPoints);
	DOREPLIFETIME_CONDITION(AUTDomGameState, TeamBodySkinColor, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AUTDomGameState, TeamSkinOverlayColor, COND_InitialOnly);
}

void AUTDomGameState::RegisterControlPoint(AControlPoint* DomObj, bool bIsDisabled)
{
	if (DomObj)
	{
		if (!bIsDisabled)
		{
			GameControlPoints.AddUnique(DomObj);
		}
		else
		{
			DomObj->DisablePoint();
		}
	}
}

void AUTDomGameState::UpdateControlPointFX(AControlPoint* ThePoint, uint8 NewTeamIndex)
{
	if (NewTeamIndex == 255)
	{
		NewTeamIndex = 4;
	}
	for (uint8 i = 0; i < GameControlPoints.Num(); i++)
	{
		if (GameControlPoints[i] == ThePoint)
		{
			GameControlPoints[i]->UpdateTeamEffects(NewTeamIndex);
			return;
		}
	}
}

AUTDomTeamInfo* AUTDomGameState::FindLeadingTeam()
{
	if (Teams[0] && Cast<AUTDomTeamInfo>(Teams[0]))
	{
		AUTDomTeamInfo* TheWinningTeam = Cast<AUTDomTeamInfo>(Teams[0]);
		for (uint8 i = 1; i < Teams.Num(); i++)
		{
			if (Teams[i] 
				&& Cast<AUTDomTeamInfo>(Teams[i]) 
				&& (Cast<AUTDomTeamInfo>(Teams[i])->GetFloatScore() > TheWinningTeam->GetFloatScore()))
			{
				TheWinningTeam = Cast<AUTDomTeamInfo>(Teams[i]);
			}
		}
		return TheWinningTeam;
	}
	return NULL;
}

void AUTDomGameState::SetWinner(AUTPlayerState* NewWinner)
{
	WinnerPlayerState = NewWinner;
	WinningTeam = NewWinner->Team; // FindLeadingTeam();
	ForceNetUpdate();
}

AUTPlayerState* AUTDomGameState::FindBestPlayerOnTeam(uint8 TeamNumToTest)
{
	AUTPlayerState* Best = NULL;
	for (uint8 i = 0; i < PlayerArray.Num(); i++)
	{
		AUTPlayerState* PS = Cast<AUTPlayerState>(PlayerArray[i]);
		if (PS != NULL && PS->GetTeamNum() == TeamNumToTest && (Best == NULL || Best->Score < PS->Score))
		{
			Best = PS;
		}
	}
	return Best;
}

FText AUTDomGameState::GetGameStatusText(bool bForScoreboard)
{
	if (bForScoreboard)
	{
		return (GoalScore == 0)
			? FText::Format(NSLOCTEXT("UTDomGameState", "GoalTimeFormat", "Score most points in {0} minutes"), FText::AsNumber(TimeLimit))
			: FText::Format(NSLOCTEXT("UTDomGameState", "GoalScoreFormat", "First Team to {0} Points"), FText::AsNumber(GoalScore));
	}

	return Super::GetGameStatusText(bForScoreboard);
}

void AUTDomGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bSkipThisTick)
	{
		bSkipThisTick = false;
	}
	else
	{
		bSkipThisTick = true;
		for (uint8 i = 0; i < PlayerArray.Num(); i++)
		{
			AUTDomPlayerState* PS = Cast<AUTDomPlayerState>(PlayerArray[i]);
			if (PS != NULL)
			{
				PS->MakeTeamSkin(PS->GetTeamNum());
			}
		}
	}
}

void AUTDomGameState::UpdateHighlights_Implementation()
{
	// add highlights to each player in order of highlight priority, filling to 5 if possible
	AUTDomPlayerState* TopScorer[4];
	AUTDomPlayerState* MostKills = NULL;
	AUTDomPlayerState* LeastDeaths = NULL;
	AUTDomPlayerState* BestKDPS = NULL;
	AUTDomPlayerState* BestComboPS = NULL;
	AUTDomPlayerState* MostHeadShotsPS = NULL;
	AUTDomPlayerState* MostAirRoxPS = NULL;

	//Collect all the weapons
	TArray<AUTWeapon *> StatsWeapons;
	if (StatsWeapons.Num() == 0)
	{
		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AUTPickupWeapon* Pickup = Cast<AUTPickupWeapon>(*It);
			if (Pickup && Pickup->GetInventoryType())
			{
				StatsWeapons.AddUnique(Pickup->GetInventoryType()->GetDefaultObject<AUTWeapon>());
			}
		}
	}

	for (uint8 i = 0; i < Teams.Num(); i++)
	{
		TopScorer[i] = Cast<AUTDomPlayerState>(FindBestPlayerOnTeam(i));
		for (TActorIterator<AUTDomPlayerState> It(GetWorld()); It; ++It)
		{
			AUTDomPlayerState* PS = *It;
			if (PS && !PS->bOnlySpectator && PS->Team && PS->Team->TeamIndex == i)
			{
				if (PS->Score > (TopScorer[i] ? TopScorer[i]->Score : 0))
				{
					TopScorer[i] = PS;
				}
				if (PS->Kills > (MostKills ? MostKills->Kills : 0))
				{
					MostKills = PS;
				}
				if (!LeastDeaths || (PS->Deaths < LeastDeaths->Deaths))
				{
					LeastDeaths = PS;
				}
				if (PS->Kills > 0)
				{
					if (!BestKDPS)
					{
						BestKDPS = PS;
					}
					else if (PS->Deaths == 0)
					{
						if ((BestKDPS->Deaths > 0) || (PS->Kills > BestKDPS->Kills))
						{
							BestKDPS = PS;
						}
					}
					else if ((BestKDPS->Deaths > 0) && (PS->Kills / PS->Deaths > BestKDPS->Kills / BestKDPS->Deaths))
					{
						BestKDPS = PS;
					}
				}

				//Figure out what weapon killed the most
				PS->FavoriteWeapon = nullptr;
				int32 BestKills = 0;
				for (AUTWeapon* Weapon : StatsWeapons)
				{
					int32 Kills = Weapon->GetWeaponKillStats(PS);
					if (Kills > BestKills)
					{
						BestKills = Kills;
						PS->FavoriteWeapon = Weapon->GetClass();
					}
				}

				if (PS->GetStatsValue(NAME_BestShockCombo) > (BestComboPS ? BestComboPS->GetStatsValue(NAME_BestShockCombo) : 0.f))
				{
					BestComboPS = PS;
				}
				if (PS->GetStatsValue(NAME_SniperHeadshotKills) > (MostHeadShotsPS ? MostHeadShotsPS->GetStatsValue(NAME_SniperHeadshotKills) : 2.f))
				{
					MostHeadShotsPS = PS;
				}
				if (PS->GetStatsValue(NAME_AirRox) > (MostAirRoxPS ? MostAirRoxPS->GetStatsValue(NAME_AirRox) : 2.f))
				{
					MostAirRoxPS = PS;
				}
			}
		}
	}
	if (BestComboPS)
	{
		BestComboPS->AddMatchHighlight(HighlightNames::BestCombo, BestComboPS->GetStatsValue(NAME_BestShockCombo));
	}
	if (BestKDPS)
	{
		BestKDPS->AddMatchHighlight(HighlightNames::BestKD, (BestKDPS->Deaths > 0) ? float(BestKDPS->Kills) / float(BestKDPS->Deaths) : BestKDPS->Kills);
	}
	TArray<float> TopScore;
	TopScore.Insert((TopScorer[0] ? TopScorer[0]->Score : 1), 0);
	TopScore.Insert((TopScorer[1] ? TopScorer[1]->Score : 1), 1);
	if (Teams.Num() > 2)
	{
		TopScore.Insert((TopScorer[2] ? TopScorer[2]->Score : 1), 2);
		if (Teams.Num() == 4)
		{
			TopScore.Insert((TopScorer[3] ? TopScorer[3]->Score : 1), 3);
		}
	}

	for (TActorIterator<AUTDomPlayerState> It(GetWorld()); It; ++It)
	{
		AUTDomPlayerState* PS = *It;
		if (PS && !PS->bOnlySpectator)
		{
			if (MostKills && (PS->Kills == MostKills->Kills))
			{
				PS->AddMatchHighlight(HighlightNames::MostKills, MostKills->Kills);
			}
			if (MostHeadShotsPS && (PS->GetStatsValue(NAME_SniperHeadshotKills) == MostHeadShotsPS->GetStatsValue(NAME_SniperHeadshotKills)))
			{
				PS->AddMatchHighlight(HighlightNames::MostHeadShots, MostHeadShotsPS->GetStatsValue(NAME_SniperHeadshotKills));
			}
			if (MostAirRoxPS && (PS->GetStatsValue(NAME_AirRox) == MostAirRoxPS->GetStatsValue(NAME_AirRox)))
			{
				PS->AddMatchHighlight(HighlightNames::MostAirRockets, MostAirRoxPS->GetStatsValue(NAME_AirRox));
			}
		}
	}

	for (TActorIterator<AUTDomPlayerState> It(GetWorld()); It; ++It)
	{
		AUTDomPlayerState* PS = *It;
		if (PS && !PS->bOnlySpectator)
		{
			// only add low priority highlights if not enough high priority highlights
			//AddMinorHighlights(PS);

			if (LeastDeaths && (PS->Deaths == LeastDeaths->Deaths))
			{
				PS->AddMatchHighlight(HighlightNames::LeastDeaths, LeastDeaths->Deaths);
			}

			//remove extra highlights
			int32 Index = 4;
			while (Index >= NumHighlightsNeeded())
			{
				if ((PS->MatchHighlights[Index] != NAME_None) && (HighlightPriority.FindRef(PS->MatchHighlights[Index]) < 2.f))
				{
					PS->MatchHighlights[Index] = NAME_None;
					PS->MatchHighlightData[Index] = 0.f;
				}
				Index--;
			}

			if (PS->MatchHighlights[0] == NAME_None)
			{
				if (PS->Kills > 0)
				{
					PS->MatchHighlights[0] = HighlightNames::KillsAward;
					PS->MatchHighlightData[0] = PS->Kills;
					if (PS->DamageDone > 0)
					{
						PS->MatchHighlights[1] = HighlightNames::DamageAward;
						PS->MatchHighlightData[1] = PS->DamageDone;
					}
				}
				else if (PS->DamageDone > 0)
				{
					PS->MatchHighlights[0] = HighlightNames::DamageAward;
					PS->MatchHighlightData[0] = PS->DamageDone;
				}
				else
				{
					PS->MatchHighlights[0] = HighlightNames::ParticipationAward;
				}
			}
			else if (PS->MatchHighlights[1] == NAME_None)
			{
				if (PS->Kills > 0)
				{
					PS->MatchHighlights[0] = HighlightNames::KillsAward;
					PS->MatchHighlightData[0] = PS->Kills;
				}
			}
		}
	}
}
