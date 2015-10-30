// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameMode.h"
#include "UTDomGameState.h"
#include "UTDomTeamInfo.h"
#include "UTDomSquadAI.h"
#include "ControlPoint.h"
#include "UTHUD_DOM.h"
#include "UTWeap_Translocator.h"

AUTDomGameMode::AUTDomGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AUTDomGameState::StaticClass();
	TeamClass = AUTDomTeamInfo::StaticClass();
	SquadType = AUTDomSquadAI::StaticClass();
	HUDClass = AUTHUD_DOM::StaticClass();
	MapPrefix = TEXT("DOM");
	bAllowOvertime = false;
	bUseTeamStarts = false;
	bAllowURLTeamCountOverride = true;
	NumTeams = 2;
	MaxSquadSize = 2;
	bAllowTranslocator = true;
	bHideInUI = false;
	//Add the translocator
	static ConstructorHelpers::FObjectFinder<UClass> WeapTranslocator(TEXT("BlueprintGeneratedClass'/Game/RestrictedAssets/Weapons/Translocator/BP_Translocator.BP_Translocator_C'"));
	DefaultInventory.Add(WeapTranslocator.Object);
}

void AUTDomGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	MaxControlPoints = FMath::Max(1, GetIntOption(Options, TEXT("MaxControlPoints"), MaxControlPoints));
	Super::InitGame(MapName, Options, ErrorMessage);
	bAllowOvertime = false;
	bUseTeamStarts = false;
	bAllowTranslocator = EvalBoolOptions(ParseOption(Options, TEXT("AllowTrans")), bAllowTranslocator);
}

void AUTDomGameMode::GameObjectiveInitialized(AUTGameObjective* Obj)
{
	Super::GameObjectiveInitialized(Obj);
	AControlPoint* DomFact = Cast<AControlPoint>(Obj);
	if (DomFact != NULL)
	{
		RegisterGameControlPoint(DomFact);
	}
}

void AUTDomGameMode::RegisterGameControlPoint(AControlPoint* DomObj)
{
	if (DomObj != NULL && CDomPoints.Num() <= MaxControlPoints && !DomObj->bHidden)
	{
		CDomPoints.AddUnique(DomObj);
		DomGameState->RegisterControlPoint(DomObj, false);
	}
}

void AUTDomGameMode::InitGameState()
{
	Super::InitGameState();
	DomGameState = Cast<AUTDomGameState>(GameState);
}

void AUTDomGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (!bAllowTranslocator)
	{
		for (int32 i = DefaultInventory.Num() - 1; i >= 0; i--)
		{
			if (DefaultInventory[i]->IsChildOf(AUTWeap_Translocator::StaticClass()))
			{
				DefaultInventory.RemoveAt(i);
			}
		}
	}
}

bool AUTDomGameMode::CheckScore_Implementation(AUTPlayerState* Scorer)
{
	AUTTeamInfo* WinningTeam = NULL;
	AUTPlayerState* BestPlayer = Scorer;
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
	return false;
}

void AUTDomGameMode::Logout(AController* Exiting)
{
	AUTPlayerState* PS = Cast<AUTPlayerState>(Exiting->PlayerState);
	if (PS != NULL && !PS->bOnlySpectator)
	{
		ClearControl(PS);
	}
	Super::Logout(Exiting);
}

/**
 *			Called when a Controller is logout from the game or changes teams. If there is
 * 			atleast one teammate left on the team, then the team will continue to be in controll
 * 			of the leaving Controllers ControlPoints.If there is no other teammates left on the
 * 			team after the player changes teams/logout, then any ControlPoints controlled by the
 * 			Controller is set back to Neutral.
 * @param	PS	The Controller who is changing teams or is logout.
 */
void AUTDomGameMode::ClearControl(AUTPlayerState* PS)
{
	TArray<AUTPlayerState*> Pick;
	uint8 Num, i;

	// find a teammate
	if (PS == NULL)
	{
		return;
	}
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AUTPlayerController* NextPlayer = Cast<AUTPlayerController>(*Iterator);
		AUTPlayerState* TestPS = NextPlayer ? Cast<AUTPlayerState>(NextPlayer->PlayerState) : NULL;
		if (TestPS != NULL && TestPS != PS && (PS->GetTeamNum() == TestPS->GetTeamNum()))
		{
			Pick.AddUnique(TestPS);
		}
	}

	if (Pick.Num() > 0)
	{
		Num = FMath::RandHelper(Pick.Num());
		if (Pick.IsValidIndex(Num) && Pick[Num] != NULL)
		{
			// Give random team mate control over leaving players control points
			for (i = 0; i < CDomPoints.Num(); i++)
			{
				if (CDomPoints[i]->ControllingTeamNum != 255 && CDomPoints[i]->ControllingPawn == PS)
				{
					CDomPoints[i]->ControllingPawn = Pick[Num];
					CDomPoints[i]->UpdateStatus();
				}
			}
			return;
		}
	}

	// No teammate found, so reset the point to X
	for (i = 0; i < CDomPoints.Num(); i++)
	{
		if (CDomPoints[i]->ControllingPawn == PS)
		{
			CDomPoints[i]->ResetPoint(true);
		}
	}
}

void AUTDomGameMode::SetEndGameFocus(AUTPlayerState* Winner)
{
	AControlPoint* WinningBase = NULL;
	if (Winner != NULL)
	{
		// find control point owned by winning player
		for (int n = 0; n < DomGameState->GameControlPoints.Num(); n++)
		{
			if (DomGameState->GameControlPoints[n]->ControllingTeam != NULL && DomGameState->GameControlPoints[n]->GetControllingTeamNum() == Winner->GetTeamNum() && DomGameState->GameControlPoints[n]->ActorIsNearMe(Winner))
			{
				WinningBase = DomGameState->GameControlPoints[n];
				break;
			}
		}
		if (WinningBase == NULL)
		{
			// find control point owned by winning player
			for (int n = 0; n < DomGameState->GameControlPoints.Num(); n++)
			{
				if ((DomGameState->GameControlPoints[n]->ControllingPawn == Winner) && (DomGameState->GameControlPoints[n]->ControllingTeam != NULL) && (DomGameState->GameControlPoints[n]->GetControllingTeamNum() == Winner->GetTeamNum()))
				{
					WinningBase = DomGameState->GameControlPoints[n];
					break;
				}
			}
		}
		// no winning player control point found, so find the first control point owned by winning team
		if (WinningBase == NULL)
		{
			for (int i = 0; i < DomGameState->GameControlPoints.Num(); i++)
			{
				if ((DomGameState->GameControlPoints[i]->ControllingTeam != NULL) && (DomGameState->GameControlPoints[i]->GetControllingTeamNum() == Winner->GetTeamNum()))
				{
					WinningBase = DomGameState->GameControlPoints[i];
					break;
				}
			}
		}
	}

	// If we don't have a winner, view 1st base
	if (WinningBase == NULL && DomGameState->GameControlPoints[0] != NULL)
	{
		WinningBase = DomGameState->GameControlPoints[0];
	}

	if (WinningBase)
	{
		EndGameFocus = WinningBase;
		EndGameFocus->bAlwaysRelevant = true;
	}

	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		AUTPlayerController* Controller = Cast<AUTPlayerController>(*Iterator);
		if ((EndGameFocus != NULL) && Controller && Controller->UTPlayerState)
		{
			Controller->GameHasEnded(EndGameFocus, Controller->UTPlayerState->Team == Winner->Team);
		}
	}
}

void AUTDomGameMode::ScoreKill_Implementation(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType)
{
	Super::AUTGameMode::ScoreKill_Implementation(Killer, Other, KilledPawn, DamageType);
}
