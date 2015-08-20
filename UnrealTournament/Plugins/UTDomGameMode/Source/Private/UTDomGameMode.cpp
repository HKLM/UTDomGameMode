// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameMode.h"
#include "UTDomGameState.h"
//#include "UTDomStats.h"
#include "UTTeamGameMode.h"
#include "UTDomTeamInfo.h"
#include "UTDomSquadAI.h"
#include "UTADomTypes.h"
#include "ControlPoint.h"
#include "DominationObjective.h"
#include "UTHUD_DOM.h"
#include "UTWeap_Translocator.h"
//#include "UTMutator.h"

DEFINE_LOG_CATEGORY(UTDomGameMode);

AUTDomGameMode::AUTDomGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AUTDomGameState::StaticClass();
	//DomStatsClass = AUTDomStats::StaticClass();
	TeamClass = AUTDomTeamInfo::StaticClass();
	SquadType = AUTDomSquadAI::StaticClass();
	HUDClass = AUTHUD_DOM::StaticClass();
	DominationObjectiveType = AControlPoint::StaticClass();
	ScoreboardClassName = FString(TEXT("/Script/UTDomGameMode.UTDomScoreboard"));
	MapPrefix = TEXT("DOM");
	bAllowOvertime = false;
	bUseTeamStarts = false;
	//bAllowURLTeamCountOverride = true;
	NumTeams = 2;
	bAllowTranslocator = true;
	bHideInUI = false;
	//Add the translocator
	static ConstructorHelpers::FObjectFinder<UClass> WeapTranslocator(TEXT("BlueprintGeneratedClass'/Game/RestrictedAssets/Weapons/Translocator/BP_Translocator.BP_Translocator_C'"));
	DefaultInventory.Add(WeapTranslocator.Object);
}

void AUTDomGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	//FActorSpawnParameters SpawnInfo;
	//SpawnInfo.Instigator = Instigator;
	//DomStats = GetWorld()->SpawnActor<AUTDomStats>(DomStatsClass, SpawnInfo);
	//if (DomGameState)
	//{
	//	DomStats->DomGameState = DomGameState;
	//}
}

void AUTDomGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	MaxControlPoints = FMath::Max(1, GetIntOption(Options, TEXT("MaxControlPoints"), MaxControlPoints));
	Super::InitGame(MapName, Options, ErrorMessage);
	bOnlyTheStrongSurvive = false;
	bAllowOvertime = false;
	bUseTeamStarts = false;
	bAllowTranslocator = EvalBoolOptions(ParseOption(Options, TEXT("AllowTrans")), bAllowTranslocator);
}

void AUTDomGameMode::GameObjectiveInitialized(AUTGameObjective* Obj)
{
	Super::GameObjectiveInitialized(Obj);
	ADominationObjective* DomFact = Cast<ADominationObjective>(Obj);
	if (DomFact != NULL)
	{
		RegisterGameControlPoint(DomFact);
	}
}

void AUTDomGameMode::RegisterGameControlPoint(ADominationObjective* DomObj)
{
	if (DomObj != NULL && DomObj->MyControlPoint != NULL && CDomPoints.Num() <= MaxControlPoints)
	{
		CDomPoints.AddUnique(DomObj->MyControlPoint);
		DomGameState->RegisterControlPoint(DomObj->MyControlPoint, false);
	}
}

void AUTDomGameMode::InitGameState()
{
	Super::InitGameState();
	DomGameState = Cast<AUTDomGameState>(GameState);
	DomGameState->DomGameObjectiveType = DominationObjectiveType;
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
		else{
			UE_LOG(UTDomGameMode, All, TEXT("!!! GAME IS IN SUDDEN DEATH !!!"));
			return false;
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
		for (int n = 0; n < CDomPoints.Num(); n++)
		{
			if (CDomPoints[n]->ControllingTeam != NULL && CDomPoints[n]->GetControllingTeamNum() == Winner->GetTeamNum() && CDomPoints[n]->HomeBase->ActorIsNearMe(Winner))
			{
				WinningBase = CDomPoints[n];
				break;
			}
		}
		if (WinningBase == NULL)
		{
			// find control point owned by winning player
			for (int n = 0; n < CDomPoints.Num(); n++)
			{
				if ((CDomPoints[n]->ControllingPawn == Winner) && (CDomPoints[n]->ControllingTeam != NULL) && (CDomPoints[n]->GetControllingTeamNum() == Winner->GetTeamNum()))
				{
					WinningBase = CDomPoints[n];
					break;
				}
			}
		}
		// no winning player control point found, so find the first control point owned by winning team
		if (WinningBase == NULL)
		{
			for (int i = 0; i < CDomPoints.Num(); i++)
			{
				if ((CDomPoints[i]->ControllingTeam != NULL) && (CDomPoints[i]->GetControllingTeamNum() == Winner->GetTeamNum()))
				{
					WinningBase = CDomPoints[i];
					break;
				}
			}
		}
	}

	//if (WinningBase != NULL)
	//{
	//	PlacePlayersAroundDomBase(Winner->GetTeamNum(), WinningBase);
	//}
	AControlPoint* BaseToView = WinningBase;
	// If we don't have a winner, view 1st base
	if (BaseToView == NULL && CDomPoints[0] != NULL)
	{
		BaseToView = CDomPoints[0];
	}

	if (BaseToView)
	{
		EndGameFocus = BaseToView;
		EndGameFocus->bAlwaysRelevant = true;
	}

	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		AUTPlayerController* Controller = Cast<AUTPlayerController>(*Iterator);
		if (Controller && Controller->UTPlayerState)
		{
			if (BaseToView != NULL)
			{
				Controller->GameHasEnded(BaseToView, Controller->UTPlayerState->Team == Winner->Team);
			}
			else
			{
				Super::SetEndGameFocus(Winner);
			}
		}
	}
}

void AUTDomGameMode::PlacePlayersAroundDomBase(int32 TeamNum, AControlPoint* DomBase)
{
	TArray<AController*> Members = Teams[TeamNum]->GetTeamMembers();
	FVector FlagLoc = DomBase->HomeBase->GetActorLocation();
	FlagLoc.Z += 50.0f;
	const int32 MaxPlayers = FMath::Min(8, Members.Num());
	float AngleSlices = 360.0f / MaxPlayers;
	int32 PlacementCounter = 0;
	for (AController* C : Members)
	{
		AUTCharacter* UTChar = C ? Cast<AUTCharacter>(C->GetPawn()) : NULL;
		if (UTChar && !UTChar->IsDead())
		{
			while (PlacementCounter < MaxPlayers)
			{
				FRotator AdjustmentAngle(0, AngleSlices * PlacementCounter, 0);

				PlacementCounter++;

				FVector PlacementLoc = FlagLoc + AdjustmentAngle.RotateVector(FVector(230, 0, 0));
				PlacementLoc.Z += UTChar->GetSimpleCollisionHalfHeight() * 1.1f;

				FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(UTChar->GetSimpleCollisionRadius(), UTChar->GetSimpleCollisionHalfHeight());
				static const FName NAME_FlagPlacement = FName(TEXT("FlagPlacement"));
				FCollisionQueryParams CapsuleParams(NAME_FlagPlacement, false, this);
				FCollisionResponseParams ResponseParam;
				TArray<FOverlapResult> Overlaps;
				bool bEncroached = GetWorld()->OverlapMultiByChannel(Overlaps, PlacementLoc, FQuat::Identity, ECC_Pawn, CapsuleShape, CapsuleParams, ResponseParam);
				if (!bEncroached)
				{
					UTChar->SetActorLocation(PlacementLoc);
					break;
				}
			}
		}

		if (PlacementCounter == 8)
		{
			break;
		}
	}
}

void AUTDomGameMode::ScoreKill_Implementation(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType)
{
	Super::AUTGameMode::ScoreKill_Implementation(Killer, Other, KilledPawn, DamageType);
}
//
//void AUTDomGameMode::BuildServerResponseRules(FString& OutRules)
//{
//	OutRules += FString::Printf(TEXT("Goal Score\t%i\t"), GoalScore);
//	OutRules += FString::Printf(TEXT("Time Limit\t%i\t"), TimeLimit);
//	OutRules += FString::Printf(TEXT("Forced Respawn\t%s\t"), bForceRespawn ? TEXT("True") : TEXT("False"));
//	OutRules += FString::Printf(TEXT("Translocator\t%s\t"), bAllowTranslocator ? TEXT("True") : TEXT("False"));
//
//	AUTMutator* Mut = BaseMutator;
//	while (Mut)
//	{
//		OutRules += FString::Printf(TEXT("Mutator\t%s\t"), *Mut->DisplayName.ToString());
//		Mut = Mut->NextMutator;
//	}
//}
