// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDoubleDomGameMode.h"
#include "UTDoubleDomGameState.h"
#include "UTDomGameMode.h"
#include "UTADomTypes.h"
#include "xDomPoint.h"
#include "DominationObjective.h"

DEFINE_LOG_CATEGORY(UTDoubleDomGameMode);

const int32 AUTDoubleDomGameMode::MAXTIMETOSCORE = 20;
const int32 AUTDoubleDomGameMode::MAXTIMEDISABLED = 20;

AUTDoubleDomGameMode::AUTDoubleDomGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AUTDoubleDomGameState::StaticClass();
	DominationObjectiveType = AxDomPoint::StaticClass();
	DisplayName = NSLOCTEXT("UTDoubleDomGameMode", "DDOM", "Double Domination");
	bAllowOvertime = true;
	bUseTeamStarts = false;
	bNoTeamPlayerStart = true;
	TimeToScore = 10;
	TimeDisabled = 10;
	GoalScore = 5;
	NumTeams = 2;
	MaxSquadSize = 2;
	MaxControlPoints = 2;
}

void AUTDoubleDomGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	bNoTeamPlayerStart = EvalBoolOptions(ParseOption(Options, TEXT("bNoTeamPlayerStart")), bNoTeamPlayerStart);
	bBalanceTeams = EvalBoolOptions(ParseOption(Options, TEXT("BalanceTeams")), bBalanceTeams);
	TimeToScore = FMath::Clamp(FMath::Min(GetIntOption(Options, TEXT("TimeToScore"), TimeToScore), MAXTIMETOSCORE), 0, MAXTIMETOSCORE);
	TimeDisabled = FMath::Clamp(FMath::Min(GetIntOption(Options, TEXT("TimeDisabled"), TimeDisabled), MAXTIMEDISABLED), 0, MAXTIMEDISABLED);
	MaxControlPoints = 2;
}

void AUTDoubleDomGameMode::InitGameState()
{
	Super::InitGameState();
	DomGameState = Cast<AUTDoubleDomGameState>(GameState);
	DomGameState->DomGameObjectiveType = DominationObjectiveType;
}

void AUTDoubleDomGameMode::GameObjectiveInitialized(AUTGameObjective* Obj)
{
	ADominationObjective* DomFact = Cast<ADominationObjective>(Obj);
	if (DomFact != NULL)
	{
		if (CDomPoints.Num() < MaxControlPoints)
		{
			DomFact->MyDomObjectiveType = DominationObjectiveType;
		}
		else
		{
			DomFact->MyDomObjectiveType = NULL;
		}
	}
	Super::AUTTeamGameMode::GameObjectiveInitialized(Obj);
	if (DomFact != NULL)
	{
		RegisterGameControlPoint(DomFact);
	}
}

void AUTDoubleDomGameMode::RegisterGameControlPoint(ADominationObjective* DomObj)
{
	if (DomObj != NULL && DomObj->MyControlPoint != NULL)
	{
		if (CDomPoints.Num() == MaxControlPoints)
		{
			// remove any additional points
			if (CDomPoints[0] != DomObj->MyControlPoint && CDomPoints[1] != DomObj->MyControlPoint)
			{
				DomObj->MyControlPoint->MyObjectiveType = EDomObjectiveType::Disabled;
				DomObj->MyControlPoint->UpdateStatus();
				DomGameState->RegisterControlPoint(DomObj->MyControlPoint, true);
				DomObj->SetActorHiddenInGame(true);
			}
		}
		else
		{
			CDomPoints.AddUnique(DomObj->MyControlPoint);
			// if this is point B, set the staticmesh to the B mesh
			if (CDomPoints.IsValidIndex(1) && CDomPoints[1] != NULL && Cast<AxDomPoint>(CDomPoints[1]))
			{
				AxDomPoint* DP = Cast<AxDomPoint>(CDomPoints[1]);
				if (DP != NULL)
				{
					DP->LetterMesh->SetStaticMesh(DP->TeamMesh[1]);
				}
			}
			DomGameState->RegisterControlPoint(DomObj->MyControlPoint, false);
		}
	}
}

void AUTDoubleDomGameMode::DefaultTimer()
{
	Super::AUTTeamGameMode::DefaultTimer();
	if (HasMatchStarted() && !HasMatchEnded())
	{
		if (CDomPoints[0] == NULL && CDomPoints[1] == NULL)
		{
			UE_LOG(UTDoubleDomGameMode, All, TEXT("ERROR!!! DDomGame::DefaultTimer  --->>> CDomPoints.Num != 2"));
			return;
		}
		AUTDoubleDomGameState* DDGS = Cast<AUTDoubleDomGameState>(DomGameState);
		if (DisabledCountDown > 0)
		{
			// Play the "New Round In" announcement
			if (!bPlayedDisabledMsg && DisabledCountDown < 7)
			{
				DDGS->SendDDomMessage(14, NULL);
				bPlayedDisabledMsg = true;
			}

			DisabledCountDown--;
			if (DisabledCountDown == 0)
			{
				CDomPoints[0]->ResetPoint(true);
				CDomPoints[1]->ResetPoint(true);
				bPlayedDisabledMsg = false;
			}
			else if (DisabledCountDown < 4) // Play the Count down announcements
			{
				DDGS->SendDDomMessage(DisabledCountDown, NULL);
			}
		}
		// nothing more to do unless the same team controls both points
		if ((CDomPoints[0]->ControllingTeam == NULL) || (CDomPoints[0]->ControllingTeam != CDomPoints[1]->ControllingTeam))
		{
			ScoreCountDown = TimeToScore;
			bPlayedTeamDominating = false;
			return;
		}

		if (ScoreCountDown > 0)
		{
			// Play the "x Team is Dominating" announcement
			if (!bPlayedTeamDominating)
			{
				DDGS->SendDDomMessage(13, CDomPoints[0]->ControllingTeam);
				bPlayedTeamDominating = true;
			}
			else if (ScoreCountDown < 8) // Play the Count down announcements
			{
				DDGS->SendDDomMessage(ScoreCountDown, NULL);
			}

			// decrement the time remaining until a point is scored
			ScoreCountDown--;
			return;
		}
		// controlling team has scored!
		ScoreDDomTeam();

		// check if the game is over
		if (bAllowOvertime && UTGameState->RemainingTime <= 0)
		{
			//TODO - add sudden death system? 
			SetMatchState(MatchState::MatchIsInOvertime);
		}
		else if (!bAllowOvertime && UTGameState->RemainingTime <= 0)
		{
			EndGame(NULL, "timelimit");
		}

		if (IsMatchInProgress() && !HasMatchEnded())
		{
			bPlayedTeamDominating = false;
			// reset count
			if (UTGameState->IsMatchInOvertime())
			{
				ScoreCountDown = (TimeToScore * 0.5f);
			}
			else
			{
				ScoreCountDown = TimeToScore;
			}
			// make all control points be untouchable for a short period
			DisabledCountDown = TimeDisabled;
			// reset both domination points and disable them
			CDomPoints[0]->ResetPoint(false);
			CDomPoints[1]->ResetPoint(false);
		}
	}
}

void AUTDoubleDomGameMode::CheckGameTime()
{
	if (IsMatchInProgress() && !HasMatchEnded() && TimeLimit > 0 && UTGameState->RemainingTime <= 0)
	{
		// Game should be over.. look to see if we need to go in to overtime....	
		uint32 bTied = 0;
		AUTPlayerState* Winner = IsThereAWinner(bTied);

		if (!bAllowOvertime || !bTied)
		{
			EndGame(Winner, FName(TEXT("TimeLimit")));
		}
		else if (bAllowOvertime && !UTGameState->IsMatchInOvertime())
		{
			// Stop the clock in Overtime. 
			UTGameState->bStopGameClock = true;
			SetMatchState(MatchState::MatchEnteringOvertime);
		}
	}
}

void AUTDoubleDomGameMode::ScoreDDomTeam()
{
	CDomPoints[0]->ControllingTeam->Score++;
	CDomPoints[0]->ControllingTeam->ForceNetUpdate();
	// play the scored music
	//BroadcastLocalizedMessage(CountdownAnnouncerMessageClass, 30);
	//PlayMusicEvent(CDomPoints[0].ControllingTeam.TeamIndex, 1);
	// Award points to the players
	for (int8 i = 0; i < MaxControlPoints; i++)
	{
		if (CDomPoints[i]->ControllingPawn != NULL)
		{
			CDomPoints[i]->ControllingPawn->AdjustScore(+5);
			// check if the game is over because either team has achieved the goal limit
			CheckScore(CDomPoints[i]->ControllingPawn);
		}
	}

	BroadcastScoreUpdate(CDomPoints[0]->ControllingPawn, CDomPoints[0]->ControllingTeam);

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AUTPlayerController* PC = Cast<AUTPlayerController>(*Iterator);
		if (PC)
		{
			PC->ClientPlaySound(AwardPointSound, 2.f);
		}
	}
}

bool AUTDoubleDomGameMode::CheckScore_Implementation(AUTPlayerState* Scorer)
{
	AUTTeamInfo* WinningTeam = NULL;
	AUTPlayerState* BestPlayer = Scorer;
	if (GoalScore > 0 && Scorer->Team->Score >= GoalScore)
	{
		EndGame(Scorer, FName(TEXT("scorelimit")));
	}
	// check if team wins by points
	if (GoalScore > 0)
	{
		for (uint8 i = 0; i < NumTeams; i++)
		{
			if (Teams[i]->Score >= GoalScore)
			{
				if (Scorer->Team != Teams[i])
				{
					BestPlayer = FindBestPlayerOnTeam(i);
				}

				DomGameState->SetWinner(BestPlayer);
				EndGame(BestPlayer, FName(TEXT("scorelimit")));
				return true;
				break;
			}
		}
	}
	// check if team wins by time limit
	if (TimeLimit > 0 && DomGameState->RemainingTime <= 0)
	{
		if (!bAllowOvertime)
		{
			AUTTeamInfo* LeadingTeam = DomGameState->FindLeadingTeam();
			if (LeadingTeam != NULL)
			{
				if (Scorer->Team != LeadingTeam)
				{
					BestPlayer = FindBestPlayerOnTeam(LeadingTeam->GetTeamNum());
				}
				DomGameState->SetWinner(BestPlayer);
				EndGame(BestPlayer, FName(TEXT("TimeLimit")));
				return true;
			}
		}
		else if (bAllowOvertime && UTGameState->IsMatchInOvertime())
		{
			UE_LOG(UTDoubleDomGameMode, All, TEXT("!!! GAME IS IN SUDDEN DEATH !!!"));
			return false;
		}
	}
	return false;
}

void AUTDoubleDomGameMode::ResetCount()
{
	if (ScoreCountDown < TimeToScore)
	{
		if (TimeToScore - ScoreCountDown > (TimeToScore / 2))
		{
			// Averted
			Cast<AUTDoubleDomGameState>(DomGameState)->SendDDomMessage(15, NULL);
		}
	}

	bPlayedTeamDominating = false;
	bPlayedDisabledMsg = false;

	if (UTGameState->IsMatchInOvertime())
	{
		ScoreCountDown = (TimeToScore * 0.5f);
	}
	else
	{
		ScoreCountDown = TimeToScore;
	}
}

//Hook to catch and calls from AControlPoint sending notifation of x point is now controlled by msg
void AUTDoubleDomGameMode::BroadcastLocalized(AActor* Sender, TSubclassOf<ULocalMessage> Message, int32 Switch, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, UObject* OptionalObject)
{
	if (OptionalObject != NULL && Cast<AControlPoint>(OptionalObject))
	{
		if (Switch == 0 || Switch == 1 || Switch == 2 || Switch == 3)
		{
			Switch = 12;
		}
	}
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		(*Iterator)->ClientReceiveLocalizedMessage(Message, Switch, RelatedPlayerState_1, RelatedPlayerState_2, OptionalObject);
	}
}
