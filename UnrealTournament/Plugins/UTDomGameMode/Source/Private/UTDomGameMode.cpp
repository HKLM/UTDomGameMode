// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTDomTeamInfo.h"
#include "UTDomSquadAI.h"
#include "ControlPoint.h"
#include "UTHUD_DOM.h"
#include "UTDomGameMessage.h"
#include "UTWeap_Translocator.h"
#include "StatNames.h"
#include "UTDomStat.h"
#include "UTFirstBloodMessage.h"
#include "UTMutator.h"
#include "Private/Slate/Widgets/SUTTabWidget.h"
#include "Private/Slate/Dialogs/SUTPlayerInfoDialog.h"
#include "SNumericEntryBox.h"
#include "UTPickup.h"
#include "UTPickupHealth.h"
#include "UTRecastNavMesh.h"
#include "UTPathBuilderInterface.h"
#include "UTDomPlayerController.h"
#include "UTDomPlayerState.h"
#include "UTDomVictoryMessage.h"
#include "UTDomGameMode.h"

AUTDomGameMode::AUTDomGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AUTDomGameState::StaticClass();
	TeamClass = AUTDomTeamInfo::StaticClass();
	SquadType = AUTDomSquadAI::StaticClass();
	HUDClass = AUTHUD_DOM::StaticClass();
	GameMessageClass = UUTDomGameMessage::StaticClass();

	PlayerPawnObject.Reset();
	PlayerPawnObject = FStringAssetReference(TEXT("/UTDomGameMode/UTDomGameContent/DefaultDomCharacter.DefaultDomCharacter_C"));

	PlayerStateClass = AUTDomPlayerState::StaticClass();
	PlayerControllerClass = AUTDomPlayerController::StaticClass();
	VictoryMessageClass = UUTDomVictoryMessage::StaticClass();
	MapPrefix = TEXT("DOM");
	bAllowOvertime = false;
	bUseTeamStarts = false;

	//TEMP DISABLED
	bBalanceTeams = false;

	bAllowURLTeamCountOverride = true;
	NumOfTeams = 4;
	NumTeams = 4;
	MaxSquadSize = 2;
	MaxControlPoints = 3;
	bAllowTranslocator = true;
	bHideInUI = false;
	//Add the translocator
	static ConstructorHelpers::FObjectFinder<UClass> WeapTranslocator(TEXT("BlueprintGeneratedClass'/Game/RestrictedAssets/Weapons/Translocator/BP_Translocator.BP_Translocator_C'"));
	DefaultInventory.Add(WeapTranslocator.Object);

	TranslocatorObject = FStringAssetReference(TEXT("/Game/RestrictedAssets/Weapons/Translocator/BP_Translocator.BP_Translocator_C"));

	TeamColors[0] = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
	TeamColors[1] = FLinearColor(0.0f, 0.0f, 1.0f, 1.0f);
	TeamColors[2] = FLinearColor(0.0f, 0.55f, 0.0f, 1.0f);
	TeamColors[3] = FLinearColor(0.6f, 0.6f, 0.0f, 1.0f);

	TeamBodySkinColor[0] = FLinearColor(4.6f, 0.1f, 0.1f, 1.0f);
	TeamBodySkinColor[1] = FLinearColor(0.1f, 0.1f, 4.6f, 1.0f);
	TeamBodySkinColor[2] = FLinearColor(0.01f, 4.1f, 0.01f, 1.0f);
	TeamBodySkinColor[3] = FLinearColor(2.6f, 2.6f, 0.01f, 1.0f);

	TeamSkinOverlayColor[0] = FLinearColor(7.0f, 0.02f, 0.02f, 1.0f);
	TeamSkinOverlayColor[1] = FLinearColor(0.04f, 0.04f, 7.4f, 1.0f);
	TeamSkinOverlayColor[2] = FLinearColor(0.02f, 6.0f, 0.02f, 1.0f);
	TeamSkinOverlayColor[3] = FLinearColor(5.5f, 5.5f, 0.02f, 1.0f);
}

void AUTDomGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::AUTGameMode::InitGame(MapName, Options, ErrorMessage);

	MaxControlPoints = FMath::Max(1, UGameplayStatics::GetIntOption(Options, TEXT("MaxControlPoints"), MaxControlPoints));
	bAllowTranslocator = EvalBoolOptions(UGameplayStatics::ParseOption(Options, TEXT("AllowTrans")), bAllowTranslocator);

	bBalanceTeams = /*!bOfflineChallenge && */EvalBoolOptions(UGameplayStatics::ParseOption(Options, TEXT("BalanceTeams")), bBalanceTeams);

	NumTeams = NumOfTeams;
	NumTeams = UGameplayStatics::GetIntOption(Options, TEXT("NumTeams"), NumTeams);
	NumTeams = FMath::Clamp<uint8>(NumTeams, 2, MAX_NUM_TEAMS);
	NumOfTeams = NumTeams;
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

		Teams.Add(NewTeam);
		checkSlow(Teams[i] == NewTeam);
	}

	bAllowOvertime = false;
	bUseTeamStarts = false;
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
			CDomPoints.AddUnique(DomObj);
			DomGameState->RegisterControlPoint(DomObj, false);
		}
		else
		{
			DomObj->DisablePoint();
		}
	}
}

void AUTDomGameMode::InitGameState()
{
	Super::InitGameState();
	DomGameState = Cast<AUTDomGameState>(GameState);
	DomGameState->NumTeams = NumTeams;
	for (uint8 i = 0; i < 4; i++)
	{
		DomGameState->TeamBodySkinColor[i] = TeamBodySkinColor[i];
		DomGameState->TeamSkinOverlayColor[i] = TeamSkinOverlayColor[i];
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

void AUTDomGameMode::GiveDefaultInventory(APawn* PlayerPawn)
{
	AUTCharacter* UTCharacter = Cast<AUTCharacter>(PlayerPawn);
	if (UTCharacter != NULL)
	{
		if (bClearPlayerInventory)
		{
			UTCharacter->DefaultCharacterInventory.Empty();
		}
		bool bFoundTranslocator = false;
		int32 foundAtIndex = -1;
		{
			// see if player has the translocator
			for (int32 i = DefaultInventory.Num() - 1; i >= 0; i--)
			{
				if (DefaultInventory[i]->IsChildOf(AUTWeap_Translocator::StaticClass()))
				{
					bFoundTranslocator = true;
					foundAtIndex = i;
				}
			}
			// if translocator is enabled, and not found, add it back
			if (bAllowTranslocator && !bFoundTranslocator)
			{
				if (!TranslocatorObject.IsNull())
				{
					TSubclassOf<AUTWeapon> WeaponClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *TranslocatorObject.ToStringReference().ToString(), NULL, LOAD_NoWarn));
					DefaultInventory.Add(WeaponClass);
				}
			}
			// if translocator is not enabled, and it is found, remove it
			else if (!bAllowTranslocator && bFoundTranslocator)
			{
				DefaultInventory.RemoveAt(foundAtIndex);
			}
		}
		UTCharacter->AddDefaultInventory(DefaultInventory);
	}
}

bool AUTDomGameMode::ChangeTeam(AController* Player, uint8 NewTeam, bool bBroadcast)
{
	if (Player == NULL)
	{
		return false;
	}
	else
	{
		AUTDomPlayerState* PS = Cast<AUTDomPlayerState>(Player->PlayerState);
		if (PS == NULL || PS->bOnlySpectator)
		{
			return false;
		}
		else
		{
			if ((bOfflineChallenge || bBasicTrainingGame) && PS->Team)
			{
				return false;
			}

			bool bForceTeam = false;
			if (!Teams.IsValidIndex(NewTeam))
			{
				bForceTeam = true;
			}
			else
			{
				// see if someone is willing to switch
				for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
				{
					AUTDomPlayerController* NextPlayer = Cast<AUTDomPlayerController>(*Iterator);
					AUTDomPlayerState* SwitchingPS = NextPlayer ? Cast<AUTDomPlayerState>(NextPlayer->PlayerState) : NULL;
					if (SwitchingPS && SwitchingPS->bPendingTeamSwitch && (SwitchingPS->Team == Teams[NewTeam]) && Teams.IsValidIndex(1-NewTeam))
					{
						// Found someone who wants to leave team, so just replace them
						MovePlayerToTeam(NextPlayer, SwitchingPS, 1 - NewTeam);
						SwitchingPS->HandleTeamChanged(NextPlayer);
						MovePlayerToTeam(Player, PS, NewTeam);
						return true;
					}
				}

				if (ShouldBalanceTeams(PS->Team == NULL))
				{
					for (int32 i = 0; i < Teams.Num(); i++)
					{
						// if this isn't smallest team, use PickBalancedTeam() to get right team
						if (i != NewTeam && Teams[i]->GetSize() <= Teams[NewTeam]->GetSize())
						{
							bForceTeam = true;
							break;
						}
					}
				}
			}
			if (bForceTeam)
			{
				NewTeam = PickBalancedTeam(PS, NewTeam);
			}
		
			if (MovePlayerToTeam(Player, PS, NewTeam))
			{
				AUTDomPlayerController* PC = Cast<AUTDomPlayerController>(Player);
				if (PC && !HasMatchStarted() && bUseTeamStarts)
				{
					AActor* const StartSpot = FindPlayerStart(PC);
					if (StartSpot != NULL)
					{
						PC->StartSpot = StartSpot;
						PC->ViewStartSpot();
					}
				}
				return true;
			}

			PS->bPendingTeamSwitch = true;
			PS->ForceNetUpdate();
			return false;
		}
	}
}

bool AUTDomGameMode::MovePlayerToTeam(AController* Player, AUTPlayerState* PS, uint8 NewTeam)
{
	AUTDomPlayerState* PSD = Cast<AUTDomPlayerState>(PS);
	if (Teams.IsValidIndex(NewTeam) && PSD && (PSD->Team == NULL || PSD->Team->TeamIndex != NewTeam))
	{
		//Make sure we kill the player before they switch sides so the correct team loses the point
		AUTCharacter* UTC = Cast<AUTCharacter>(Player->GetPawn());
		if (UTC != nullptr)
		{
			UTC->PlayerSuicide();
		}
		if (PSD->Team != NULL)
		{
			PSD->Team->RemoveFromTeam(Player);
		}
		Teams[NewTeam]->AddToTeam(Player);
		PSD->bPendingTeamSwitch = false;
		PSD->ForceNetUpdate();

		PSD->MakeTeamSkin(NewTeam);
		// Clear the player's gameplay mute list.

		AUTDomPlayerController* PlayerController = Cast<AUTDomPlayerController>(Player);
		AUTDomGameState* MyGameState = GetWorld()->GetGameState<AUTDomGameState>();

		if (PlayerController && MyGameState)
		{
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				AUTDomPlayerController* NextPlayer = Cast<AUTDomPlayerController>(*Iterator);
				if (NextPlayer)
				{
					TSharedPtr<const FUniqueNetId> Id = NextPlayer->PlayerState->UniqueId.GetUniqueNetId();
					bool bIsMuted = Id.IsValid() && PlayerController->IsPlayerMuted(Id.ToSharedRef().Get());

					bool bOnSameTeam = MyGameState->OnSameTeam(PlayerController, NextPlayer);
					if (bIsMuted && bOnSameTeam) 
					{
						PlayerController->GameplayUnmutePlayer(NextPlayer->PlayerState->UniqueId);
						NextPlayer->GameplayUnmutePlayer(PlayerController->PlayerState->UniqueId);
					}
					if (!bIsMuted && !bOnSameTeam) 
					{
						PlayerController->GameplayMutePlayer(NextPlayer->PlayerState->UniqueId);
						NextPlayer->GameplayMutePlayer(PlayerController->PlayerState->UniqueId);
					}
					
				}
			}
		}

		return true;
	}
	return false;
}

bool AUTDomGameMode::CheckScore_Implementation(AUTPlayerState* Scorer)
{
	AUTDomTeamInfo* WinningTeam = NULL;
	AUTPlayerState* BestPlayer = Scorer;
	// check if team wins by points
	if (GoalScore != 0)
	{
		for (uint8 i = 0; i < NumTeams; i++)
		{
			if (Teams.IsValidIndex(i) 
				&& Cast<AUTDomTeamInfo>(Teams[i]) != NULL
				&& Cast<AUTDomTeamInfo>(Teams[i])->GetFloatScore() >= GoalScore)
			{
				BestPlayer = FindBestPlayerOnTeam(i);
				if (BestPlayer == NULL && Scorer->Team == Teams[i])
				{
					BestPlayer = Scorer;
				}
				//DomGameState->SetWinner(BestPlayer);
				EndGame(BestPlayer, FName(TEXT("scorelimit")));
				return true;
			}
		}
	}
	// check if team wins by time limit
	if (TimeLimit != 0 && DomGameState->GetRemainingTime() <= 0)
	{
		AUTDomTeamInfo* LeadingTeam = DomGameState->FindLeadingTeam();
		if (LeadingTeam != NULL)
		{
			BestPlayer = FindBestPlayerOnTeam(LeadingTeam->GetTeamNum());
			if (BestPlayer == NULL && Scorer->GetTeamNum() == LeadingTeam->GetTeamNum())
			{
				BestPlayer = Scorer;
			}
			//DomGameState->SetWinner(BestPlayer);
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

void AUTDomGameMode::EndGame(AUTPlayerState* Winner, FName Reason)
{
	for (int8 i = 0; i < CDomPoints.Num(); i++)
	{
		CDomPoints[i]->bStopControlledTimer = true;
	}
	Super::EndGame(Winner, Reason);
}

void AUTDomGameMode::SetEndGameFocus(AUTPlayerState* Winner)
{
	AControlPoint* WinningBase = nullptr;
	AUTDomTeamInfo* WinningTeam = (Winner && Winner->Team) ? Cast<AUTDomTeamInfo>(Winner->Team) : DomGameState->FindLeadingTeam();
	if (Winner && DomGameState->GameControlPoints.Num() > 0)
	{
		// find control point owned by winning player
		for (uint8 n = 0; n < DomGameState->GameControlPoints.Num(); n++)
		{
			if ((DomGameState->GameControlPoints[n] && DomGameState->GameControlPoints[n]->ControllingTeam && DomGameState->GameControlPoints[n]->TeamNum == WinningTeam->GetTeamNum()) && DomGameState->GameControlPoints[n]->ActorIsNearMe(Winner))
			{
				WinningBase = DomGameState->GameControlPoints[n];
				break;
			}
		}
		if (WinningBase == nullptr)
		{
			// find control point owned by winning player
			for (uint8 n = 0; n < DomGameState->GameControlPoints.Num(); n++)
			{
				if (DomGameState->GameControlPoints[n] && DomGameState->GameControlPoints[n]->ControllingPawn)
				{
					if ((DomGameState->GameControlPoints[n]->ControllingPawn == Winner) && (DomGameState->GameControlPoints[n]->ControllingTeam != NULL) && (DomGameState->GameControlPoints[n]->ControllingTeam->TeamIndex == WinningTeam->GetTeamNum()))
					{
						WinningBase = DomGameState->GameControlPoints[n];
						break;
					}
				}
			}
		}
		// no winning player control point found, so find the first control point owned by winning team
		if (WinningBase == nullptr)
		{
			for (uint8 i = 0; i < DomGameState->GameControlPoints.Num(); i++)
			{
				if ((DomGameState->GameControlPoints[i]->ControllingTeam != NULL) && (DomGameState->GameControlPoints[i]->ControllingTeam->TeamIndex == WinningTeam->GetTeamNum()))
				{
					WinningBase = DomGameState->GameControlPoints[i];
					break;
				}
			}
		}
	}

	// If we don't have a winner, something must be wrong, just view 1st base
	if (WinningBase == nullptr && DomGameState->GameControlPoints[0] != NULL)
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
		AUTDomPlayerController* Controller = Cast<AUTDomPlayerController>(*Iterator);
		if (Controller && Controller->UTPlayerState && Controller->UTPlayerState->Team)
		{
			Controller->GameHasEnded(EndGameFocus, (Controller->UTPlayerState->Team->TeamIndex == WinningTeam->GetTeamNum()));
		}
	}
}

void AUTDomGameMode::PlayEndOfMatchMessage()
{
	if (UTGameState && UTGameState->WinningTeam)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			AUTPlayerController* PC = Cast<AUTPlayerController>(*Iterator);
			if (PC && (PC->PlayerState != NULL) && !PC->PlayerState->bOnlySpectator)
			{
				PC->ClientReceiveLocalizedMessage(VictoryMessageClass,
													UTGameState->WinningTeam->GetTeamNum(),
													UTGameState->WinnerPlayerState,
													PC->PlayerState,
													UTGameState->WinningTeam);
			}
		}
	}
}

void AUTDomGameMode::ScoreKill_Implementation(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType)
{
	AUTPlayerState* OtherPlayerState = Other ? Cast<AUTPlayerState>(Other->PlayerState) : NULL;
	if ((Killer == Other) || (Killer == NULL))
	{
		// If it's a suicide, subtract a kill from the player...
		if (OtherPlayerState)
		{
			OtherPlayerState->AdjustScore(-1);
		}
	}
	else
	{
		AUTPlayerState * KillerPlayerState = Cast<AUTPlayerState>(Killer->PlayerState);
		if (KillerPlayerState != NULL)
		{
			KillerPlayerState->AdjustScore(+1);
			KillerPlayerState->IncrementKills(DamageType, true, OtherPlayerState);
			KillerPlayerState->ModifyStatsValue(NAME_RegularKillPoints, 1);
		}

		if (!bFirstBloodOccurred)
		{
			BroadcastLocalized(this, UUTFirstBloodMessage::StaticClass(), 0, KillerPlayerState, NULL, NULL);
			bFirstBloodOccurred = true;
		}
	}

	AddKillEventToReplay(Killer, Other, DamageType);

	if (BaseMutator != NULL)
	{
		BaseMutator->ScoreKill(Killer, Other, DamageType);
	}
	FindAndMarkHighScorer();
}

void AUTDomGameMode::BuildServerResponseRules(FString& OutRules)
{
	OutRules += FString::Printf(TEXT("Goal Score\t%i\t"), GoalScore);
	OutRules += FString::Printf(TEXT("Time Limit\t%i\t"), int32(TimeLimit / 60.0));
	OutRules += FString::Printf(TEXT("Teams\t%i\t"), NumTeams);
	OutRules += FString::Printf(TEXT("Translocator\t%s\t"), bAllowTranslocator ? TEXT("True") : TEXT("False"));

	AUTMutator* Mut = BaseMutator;
	while (Mut)
	{
		OutRules += FString::Printf(TEXT("Mutator\t%s\t"), *Mut->DisplayName.ToString());
		Mut = Mut->NextMutator;
	}
}

void AUTDomGameMode::CreateGameURLOptions(TArray<TSharedPtr<TAttributePropertyBase>>& MenuProps)
{
	Super::CreateGameURLOptions(MenuProps);
	MenuProps.Add(MakeShareable(new TAttributePropertyBool(this, &bAllowTranslocator, TEXT("AllowTrans"))));
	MenuProps.Add(MakeShareable(new TAttributeProperty<int32>(this, &MaxControlPoints, TEXT("MaxControlPoints"))));
	MenuProps.Add(MakeShareable(new TAttributeProperty<int32>(this, &NumOfTeams, TEXT("NumTeams"))));
}

#if !UE_SERVER
void AUTDomGameMode::CreateConfigWidgets(TSharedPtr<class SVerticalBox> MenuSpace, bool bCreateReadOnly, TArray< TSharedPtr<TAttributePropertyBase> >& ConfigProps, int32 MinimumPlayers)
{
	Super::CreateConfigWidgets(MenuSpace, bCreateReadOnly, ConfigProps, MinimumPlayers);

	TSharedPtr< TAttributePropertyBool > AllowTransAttr = StaticCastSharedPtr<TAttributePropertyBool>(FindGameURLOption(ConfigProps, TEXT("AllowTrans")));
	TSharedPtr< TAttributeProperty<int32> > NumOfTeamsAttr = StaticCastSharedPtr<TAttributeProperty<int32>>(FindGameURLOption(ConfigProps, TEXT("NumTeams")));

	if (AllowTransAttr.IsValid())
	{
		MenuSpace->AddSlot()
			.Padding(0.0f, 0.0f, 0.0f, 5.0f)
			.AutoHeight()
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
				.Text(NSLOCTEXT("UTDomGameMode", "AllowTranslocator", "Translocator"))
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
					SNew(SCheckBox)
					.IsChecked(AllowTransAttr.ToSharedRef(), &TAttributePropertyBool::GetAsCheckBox)
					.Type(ESlateCheckBoxType::CheckBox)
					.Style(SUWindowsStyle::Get(), "UT.Common.CheckBox")
					) :
			StaticCastSharedRef<SWidget>(
				SNew(SCheckBox)
				.IsChecked(AllowTransAttr.ToSharedRef(), &TAttributePropertyBool::GetAsCheckBox)
				.OnCheckStateChanged(AllowTransAttr.ToSharedRef(), &TAttributePropertyBool::SetFromCheckBox)
				.Type(ESlateCheckBoxType::CheckBox)
				.Style(SUWindowsStyle::Get(), "UT.Common.CheckBox")
				)
			]
			]
			];
	}
	if (NumOfTeamsAttr.IsValid())
	{
		MenuSpace->AddSlot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.Padding(0.0f,0.0f,0.0f,5.0f)
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
					.TextStyle(SUWindowsStyle::Get(),"UT.Common.NormalText")
					.Text(NSLOCTEXT("UTDomGameMode", "NumTeams", "Teams"))
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(20.0f,0.0f,0.0f,0.0f)
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(300)
				[
					bCreateReadOnly ?
					StaticCastSharedRef<SWidget>(
						SNew(STextBlock)
						.TextStyle(SUWindowsStyle::Get(),"UT.Common.ButtonText.White")
						.Text(NumOfTeamsAttr.ToSharedRef(), &TAttributeProperty<int32>::GetAsText)
					) :
					StaticCastSharedRef<SWidget>(
						SNew(SNumericEntryBox<int32>)
						.Value(NumOfTeamsAttr.ToSharedRef(), &TAttributeProperty<int32>::GetOptional)
						.OnValueChanged(NumOfTeamsAttr.ToSharedRef(), &TAttributeProperty<int32>::Set)
						.AllowSpin(true)
						.Delta(1)
						.MinValue(2)
						.MaxValue(MAX_NUM_TEAMS)
						.MinSliderValue(2)
						.MaxSliderValue(MAX_NUM_TEAMS)
						.EditableTextBoxStyle(SUWindowsStyle::Get(), "UT.Common.NumEditbox.White")
					)
				]
			]
		];
	}
}

void AUTDomGameMode::BuildScoreInfo(AUTPlayerState* PlayerState, TSharedPtr<class SUTTabWidget> TabWidget, TArray<TSharedPtr<TAttributeStat> >& StatList)
{
	TAttributeStat::StatValueTextFunc TwoDecimal = [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> FText
	{
		return FText::FromString(FString::Printf(TEXT("%8.2f"), Stat->GetValue()));
	};

	TSharedPtr<SVerticalBox> LeftPane;
	TSharedPtr<SVerticalBox> RightPane;
	TSharedPtr<SHorizontalBox> HBox;
	BuildPaneHelper(HBox, LeftPane, RightPane);

	TabWidget->AddTab(NSLOCTEXT("AUTGameMode", "Score", "Score"), HBox);

	NewPlayerInfoLine(LeftPane, NSLOCTEXT("ADomination", "Score", "Player Score"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float { return PS->Score;	})), StatList);

	LeftPane->AddSlot().AutoHeight()[SNew(SBox).HeightOverride(40.0f)];
	LeftPane->AddSlot().AutoHeight()[SNew(SBox)
		.HeightOverride(50.0f)
		[
			SNew(STextBlock)
			.TextStyle(SUWindowsStyle::Get(), "UT.Common.BoldText")
		.Text(NSLOCTEXT("ADomination", "Scoring", " SCORING "))
		]
	];

	NewPlayerInfoLine(LeftPane, NSLOCTEXT("ADomination", "RegularKillPoints", "Score from Frags"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float { return PS->GetStatsValue(NAME_RegularKillPoints);	})), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTGameMode", "Kills", "Kills"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float { return PS->Kills;	})), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTGameMode", "Deaths", "Deaths"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float {	return PS->Deaths; })), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTGameMode", "Suicides", "Suicides"), MakeShareable(new TAttributeStat(PlayerState, NAME_Suicides)), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTGameMode", "ScorePM", "Score Per Minute"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float
	{
		return (PS->StartTime < PS->GetWorld()->GetGameState<AUTDomGameState>()->ElapsedTime) ? PS->Score * 60.f / (PS->GetWorld()->GetGameState<AUTDomGameState>()->ElapsedTime - PS->StartTime) : 0.f;
	}, TwoDecimal)), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTGameMode", "KDRatio", "K/D Ratio"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float
	{
		return (PS->Deaths > 0) ? float(PS->Kills) / PS->Deaths : 0.f;
	}, TwoDecimal)), StatList);

	LeftPane->AddSlot().AutoHeight()[SNew(SBox).HeightOverride(40.0f)];
	LeftPane->AddSlot().AutoHeight()[SNew(SBox)
		.HeightOverride(50.0f)
		[
			SNew(STextBlock)
			.TextStyle(SUWindowsStyle::Get(), "UT.Common.BoldText")
		.Text(NSLOCTEXT("ADomination", "ControlPointStats", " CONTROL POINTS STATS "))
		]
	];

	TAttributeStat::StatValueTextFunc ToTime = [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> FText
	{
		int32 Seconds = (int32)Stat->GetValue();
		int32 Mins = Seconds / 60;
		Seconds -= Mins * 60;
		return FText::FromString(FString::Printf(TEXT("%d:%02d"), Mins, Seconds));
	};

	NewPlayerInfoLine(LeftPane, NSLOCTEXT("ADomination", "ControlPointHeldPoints", "Points from Capture"), MakeShareable(new TAttributeStat(PlayerState, NAME_ControlPointHeldPoints)), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("ADomination", "ControlPointCaps", "Captures"), MakeShareable(new TAttributeStat(PlayerState, NAME_ControlPointCaps)), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("ADomination", "ControlPointHeldTime", "Total Held Time"), MakeShareable(new TAttributeStat(PlayerState, NAME_ControlPointHeldTime, nullptr, ToTime)), StatList);
}

#endif
