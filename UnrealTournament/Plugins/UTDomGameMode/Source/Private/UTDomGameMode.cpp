// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTDomTeamInfo.h"
#include "UTDomSquadAI.h"
#include "ControlPoint.h"
#include "UTHUD_DOM.h"
#include "UTDomGameMessage.h"
#include "UTDomPlayerController.h"
#include "UTDomPlayerState.h"
#include "UTWeap_Translocator.h"
#include "Private/Slate/Widgets/SUTTabWidget.h"
#include "SNumericEntryBox.h"
#include "UTDomGameMode.h"

AUTDomGameMode::AUTDomGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AUTDomGameState::StaticClass();
	TeamClass = AUTDomTeamInfo::StaticClass();
	SquadType = AUTDomSquadAI::StaticClass();
	HUDClass = AUTHUD_DOM::StaticClass();
	GameMessageClass = UUTDomGameMessage::StaticClass();
	PlayerStateClass = AUTDomPlayerState::StaticClass();
	PlayerControllerClass = AUTDomPlayerController::StaticClass();
	MapPrefix = TEXT("DOM");
	bAllowOvertime = false;
	bUseTeamStarts = false;
	bAllowURLTeamCountOverride = true;
	NumTeams = 2;
	MaxNumTeams = 4;
	MaxSquadSize = 2;
	bAllowTranslocator = true;
	bHideInUI = false;
	//Add the translocator
	static ConstructorHelpers::FObjectFinder<UClass> WeapTranslocator(TEXT("BlueprintGeneratedClass'/Game/RestrictedAssets/Weapons/Translocator/BP_Translocator.BP_Translocator_C'"));
	DefaultInventory.Add(WeapTranslocator.Object);

	// Team Skin overlays
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TOverlay0(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Character/Skins/DomTeamSkinOverlay_Inst0.DomTeamSkinOverlay_Inst0'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TOverlay1(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Character/Skins/DomTeamSkinOverlay_Inst1.DomTeamSkinOverlay_Inst1'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TOverlay2(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Character/Skins/DomTeamSkinOverlay_Inst2.DomTeamSkinOverlay_Inst2'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TOverlay3(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Character/Skins/DomTeamSkinOverlay_Inst3.DomTeamSkinOverlay_Inst3'"));
	TeamOverlayEffect.Insert(TOverlay0.Object, 0);
	TeamOverlayEffect.Insert(TOverlay1.Object, 1);
	TeamOverlayEffect.Insert(TOverlay2.Object, 2);
	TeamOverlayEffect.Insert(TOverlay3.Object, 3);

	TeamColors[0] = FLinearColor(1.15f, 0.0f, 0.0f, 0.72f);
	TeamColors[1] = FLinearColor(0.0f, 0.0f, 1.2f, 0.72f);
	TeamColors[2] = FLinearColor(0.0f, 1.20f, 0.0f, 0.72f);
	TeamColors[3] = FLinearColor(1.15f, 1.15f, 0.0f, 0.72f);
}

void AUTDomGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::AUTGameMode::InitGame(MapName, Options, ErrorMessage);

	MaxControlPoints = FMath::Max(1, GetIntOption(Options, TEXT("MaxControlPoints"), MaxControlPoints));
	/*!	Override AUTTeamGameMode::InitGame due to hardcoding of max 2 teams
	@note NumTeams = FMath::Max<uint8>(NumTeams, 2);
	*/
	bBalanceTeams = /*!bOfflineChallenge && */EvalBoolOptions(ParseOption(Options, TEXT("BalanceTeams")), bBalanceTeams);

	NumTeams = GetIntOption(Options, TEXT("NumTeams"), NumTeams);
	NumTeams = FMath::Clamp<uint8>(NumTeams, 2, MaxNumTeams);
	if (NumTeams > 2)
	{
		//remove uneeded items from TeamOverlayEffect array to NumTeams size
		TeamOverlayEffect.SetNum(NumTeams, true);
	}
	else
	{
		//Dont use for the default 2 team play
		TeamOverlayEffect.Empty();
	}
	if (TeamClass == NULL)
	{
		TeamClass = AUTDomTeamInfo::StaticClass();
	}
	for (uint8 i = 0; i < NumTeams; i++)
	{
		AUTDomTeamInfo* NewTeam = GetWorld()->SpawnActor<AUTDomTeamInfo>(TeamClass);
		NewTeam->TeamIndex = i;
		if (TeamColors.IsValidIndex(i))
		{
			NewTeam->TeamColor = TeamColors[i];
		}
		if (TeamNames.IsValidIndex(i))
		{
			NewTeam->TeamName = TeamNames[i];
		}
		if (TeamOverlayEffect.Num() > 0 && TeamOverlayEffect.IsValidIndex(i))
		{
			NewTeam->TeamSkinOverlay = TeamOverlayEffect[i];
		}

		Teams.Add(NewTeam);
		checkSlow(Teams[i] == NewTeam);
	}

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
	if (DomObj != NULL)
	{
		if (CDomPoints.Num() <= MaxControlPoints && !DomObj->bHidden)
		{
			DomObj->DomGameState = DomGameState;
			CDomPoints.AddUnique(DomObj);
			DomGameState->RegisterControlPoint(DomObj, false);
		}
		else
		{
			DomObj->bHidden = true;
			DomObj->DisablePoint();
		}
	}
}

void AUTDomGameMode::InitGameState()
{
	Super::InitGameState();
	DomGameState = Cast<AUTDomGameState>(GameState);
	DomGameState->NumTeams = NumTeams;
	if (NumTeams > 2)
	{
		//Bright Team skins
		for (uint8 i = 0; i < TeamOverlayEffect.Num(); i++)
		{
			if (TeamOverlayEffect[i] != NULL
				&& Teams.IsValidIndex(i)
				&& DomGameState != NULL)
			{
				AUTDomTeamInfo* DTI = Cast<AUTDomTeamInfo>(Teams[i]);
				DomGameState->AddOverlayMaterial(TeamOverlayEffect[i], TeamOverlayEffect[i]);
				DTI->TeamSkinOverlay = TeamOverlayEffect[i];
			}
		}
	}
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

void AUTDomGameMode::AnnounceMatchStart()
{
	if (bAnnounceTeam)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			AUTPlayerController* NextPlayer = Cast<AUTPlayerController>(*Iterator);
			AUTTeamInfo* Team = (NextPlayer && Cast<AUTPlayerState>(NextPlayer->PlayerState)) ? Cast<AUTPlayerState>(NextPlayer->PlayerState)->Team : NULL;
			if (Team)
			{
				NextPlayer->ClientReceiveLocalizedMessage(GameMessageClass, Team->TeamIndex + 9, NextPlayer->PlayerState, NULL, NULL);
			}
		}
	}
	else
	{
		BroadcastLocalized(this, UUTGameMessage::StaticClass(), 0, NULL, NULL, NULL);
	}
}

// Use Team skins for 3/4 team play
void AUTDomGameMode::RestartPlayer(AController* aPlayer)
{
	Super::RestartPlayer(aPlayer);
	if ((aPlayer == NULL) || (aPlayer->PlayerState == NULL) || aPlayer->PlayerState->PlayerName.IsEmpty() || !IsMatchInProgress() || aPlayer->PlayerState->bOnlySpectator)
	{
		return;
	}
	if (NumTeams > 2)
	{
		if (Cast<AUTPlayerState>(aPlayer->PlayerState) != NULL && Cast<AUTDomTeamInfo>(Cast<AUTPlayerState>(aPlayer->PlayerState)->Team) != NULL)
		{
			AUTDomTeamInfo* DTI = Cast<AUTDomTeamInfo>(Cast<AUTPlayerState>(aPlayer->PlayerState)->Team);
			if (DTI != NULL)
			{
				Cast<AUTPlayerState>(aPlayer->PlayerState)->GetUTCharacter()->SetCharacterOverlayEffect(FOverlayEffect(DTI->TeamSkinOverlay), true);
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
				if (CDomPoints[i]->TeamNum != 255 && CDomPoints[i]->ControllingPawn == PS)
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
			if (DomGameState->GameControlPoints[n]->ControllingTeam != NULL && DomGameState->GameControlPoints[n]->TeamNum == Winner->GetTeamNum() && DomGameState->GameControlPoints[n]->ActorIsNearMe(Winner))
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
				if ((DomGameState->GameControlPoints[n]->ControllingPawn == Winner) && (DomGameState->GameControlPoints[n]->ControllingTeam != NULL) && (DomGameState->GameControlPoints[n]->TeamNum == Winner->GetTeamNum()))
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
				if ((DomGameState->GameControlPoints[i]->ControllingTeam != NULL) && (DomGameState->GameControlPoints[i]->TeamNum == Winner->GetTeamNum()))
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

void AUTDomGameMode::CreateGameURLOptions(TArray<TSharedPtr<TAttributePropertyBase>>& MenuProps)
{
	Super::CreateGameURLOptions(MenuProps);
	MenuProps.Add(MakeShareable(new TAttributeProperty<uint8>(this, &NumTeams, TEXT("NumTeams"))));
}

#if !UE_SERVER
void AUTDomGameMode::CreateConfigWidgets(TSharedPtr<class SVerticalBox> MenuSpace, bool bCreateReadOnly, TArray< TSharedPtr<TAttributePropertyBase> >& ConfigProps)
{
	Super::CreateConfigWidgets(MenuSpace, bCreateReadOnly, ConfigProps);

	TSharedPtr< TAttributeProperty<uint8> > NumTeamsAttr = StaticCastSharedPtr<TAttributeProperty<uint8>>(FindGameURLOption(ConfigProps, TEXT("NumTeams")));
	if (NumTeamsAttr.IsValid())
	{
		MenuSpace->AddSlot()
			.AutoHeight()
			.VAlign(VAlign_Top)
			.Padding(0.0f, 0.0f, 0.0f, 5.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(350)
					[
						SNew(STextBlock)
						.TextStyle(SUWindowsStyle::Get(), "UT.Common.NormalText")
						.Text(NSLOCTEXT("UTDomGameMode", "NumTeams", "EXPERIMENTAL-# of Teams"))
					]
				]
				+ SHorizontalBox::Slot()
					.Padding(20.0f, 0.0f, 0.0f, 0.0f)
					.AutoWidth()
					[
						SNew(SBox)
						.WidthOverride(300)
						[
							bCreateReadOnly ?
							StaticCastSharedRef<SWidget>(
							SNew(STextBlock)
							.TextStyle(SUWindowsStyle::Get(), "UT.Common.ButtonText.White")
							.Text(NumTeamsAttr.ToSharedRef(), &TAttributeProperty<uint8>::GetAsText)
							) :
							StaticCastSharedRef<SWidget>(
							SNew(SNumericEntryBox<uint8>)
							.Value(NumTeamsAttr.ToSharedRef(), &TAttributeProperty<uint8>::GetOptional)
							.OnValueChanged(NumTeamsAttr.ToSharedRef(), &TAttributeProperty<uint8>::Set)
							.AllowSpin(true)
							.Delta(1)
							.MinValue(2)
							.MaxValue(4)
							.MinSliderValue(2)
							.MaxSliderValue(4)
							.EditableTextBoxStyle(SUWindowsStyle::Get(), "UT.Common.NumEditbox.White")
							)
						]
					]
			];
	}
}
#endif

