// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "MultiTeamTeamInfo.h"
#include "MultiTeamSquadAI.h"
#include "ControlPoint.h"
#include "UTHUD_DOM.h"
#include "UTDomGameMessage.h"
#include "UTWeap_Translocator.h"
#include "StatNames.h"
#include "UTDomStat.h"
#include "UTADomTypes.h"
#include "UTFirstBloodMessage.h"
#include "UTMutator.h"
#include "Private/Slate/Widgets/SUTTabWidget.h"
#include "Private/Slate/Dialogs/SUTPlayerInfoDialog.h"
#include "SNumericEntryBox.h"
#include "UTPickup.h"
#include "UTPickupHealth.h"
#include "UTRecastNavMesh.h"
#include "UTPathBuilderInterface.h"
#include "MultiTeamPlayerController.h"
#include "MultiTeamPlayerState.h"
#include "MultiTeamVictoryMessage.h"
#include "UTDomEndFocusActor.h"
#include "MultiTeamGameMode.h"
#include "UTDomGameMode.h"

AUTDomGameMode::AUTDomGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AUTDomGameState::StaticClass();
	SquadType = AMultiTeamSquadAI::StaticClass();
	HUDClass = AUTHUD_DOM::StaticClass();
	GameMessageClass = UUTDomGameMessage::StaticClass();

	MapPrefix = TEXT("DOM");
	bAllowOvertime = false;
	bUseTeamStarts = false;

	//TEMP DISABLED
	bBalanceTeams = false;

	NumOfTeams = 4;
	NumTeams = 4;
	MaxSquadSize = 2;
	MaxControlPoints = 3;
	bGameHasImpactHammer = true;
	bAllowTranslocator = true;
	bHideInUI = false;
	//Add the translocator
	static ConstructorHelpers::FObjectFinder<UClass> WeapTranslocator(TEXT("BlueprintGeneratedClass'/Game/RestrictedAssets/Weapons/Translocator/BP_Translocator.BP_Translocator_C'"));
	DefaultInventory.Add(WeapTranslocator.Object);

	TranslocatorObject = FStringAssetReference(TEXT("/Game/RestrictedAssets/Weapons/Translocator/BP_Translocator.BP_Translocator_C"));
}

void AUTDomGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	MaxControlPoints = FMath::Max(1, UGameplayStatics::GetIntOption(Options, TEXT("MaxControlPoints"), MaxControlPoints));
	bAllowTranslocator = EvalBoolOptions(UGameplayStatics::ParseOption(Options, TEXT("AllowTrans")), bAllowTranslocator);

	bAllowOvertime = false;
	bUseTeamStarts = false;
}

void AUTDomGameMode::PostInitializeComponents()
{
	for (TActorIterator<AUTGameObjective> ObjIt(GetWorld()); ObjIt; ++ObjIt)
	{
		AControlPoint* CPitem = Cast<AControlPoint>(*ObjIt);
		if (CPitem && CPitem->GetIsGameObjective())
		{
			RegisterGameControlPoint(CPitem);
		}
	}
	Super::PostInitializeComponents();
}

void AUTDomGameMode::RegisterGameControlPoint(AControlPoint* DomObj)
{
	if (DomObj != nullptr)
	{
		if (CDomPoints.Num() <= MaxControlPoints && !DomObj->bHidden/* && DomObj->GetIsGameObjective()*/)
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
//
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

void AUTDomGameMode::GiveDefaultInventory(APawn* PlayerPawn)
{
	AUTCharacter* UTCharacter = Cast<AUTCharacter>(PlayerPawn);
	if (UTCharacter != nullptr)
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

bool AUTDomGameMode::CheckScore_Implementation(AUTPlayerState* Scorer)
{
	AMultiTeamTeamInfo* WinningTeam = nullptr;
	AUTPlayerState* BestPlayer = Scorer;
	// check if team wins by points
	if (GoalScore != 0)
	{
		for (uint8 i = 0; i < NumTeams; i++)
		{
			if (Teams.IsValidIndex(i) 
				&& Cast<AMultiTeamTeamInfo>(Teams[i]) != NULL
				&& Cast<AMultiTeamTeamInfo>(Teams[i])->GetFloatScore() >= GoalScore)
			{
				BestPlayer = FindBestPlayerOnTeam(i);
				if (BestPlayer == nullptr && Scorer->Team == Teams[i])
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
		AMultiTeamTeamInfo* LeadingTeam = DomGameState->FindLeadingTeam();
		if (LeadingTeam != nullptr)
		{
			BestPlayer = FindBestPlayerOnTeam(LeadingTeam->GetTeamNum());
			if (BestPlayer == nullptr && Scorer->GetTeamNum() == LeadingTeam->GetTeamNum())
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
	if (PS != nullptr && !PS->bOnlySpectator)
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
	if (PS == nullptr)
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
	AMultiTeamTeamInfo* WinningTeam = (Winner && Winner->Team) ? Cast<AMultiTeamTeamInfo>(Winner->Team) : DomGameState->FindLeadingTeam();
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
					if ((DomGameState->GameControlPoints[n]->ControllingPawn == Winner) && (DomGameState->GameControlPoints[n]->ControllingTeam != nullptr) && (DomGameState->GameControlPoints[n]->ControllingTeam->TeamIndex == WinningTeam->GetTeamNum()))
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
				if ((DomGameState->GameControlPoints[i]->ControllingTeam != nullptr) && (DomGameState->GameControlPoints[i]->ControllingTeam->TeamIndex == WinningTeam->GetTeamNum()))
				{
					WinningBase = DomGameState->GameControlPoints[i];
					break;
				}
			}
		}
	}

	// If we don't have a winner, something must be wrong, just view 1st base
	if (WinningBase == nullptr && DomGameState->GameControlPoints[0] != nullptr)
	{
		WinningBase = DomGameState->GameControlPoints[0];
	}

	if (WinningBase)
	{
		//Move focus up as to prevent bug of viewing the EndGameFocus underground
		FActorSpawnParameters Params;
		Params.Owner = WinningBase;
		AUTDomEndFocusActor* EndFocus = GetWorld()->SpawnActor<AUTDomEndFocusActor>(AUTDomEndFocusActor::StaticClass(), WinningBase->GetActorLocation() + FVector(0.0f, 0.0f, 25.0f), WinningBase->GetActorRotation(), Params);
		if (EndFocus)
		{
			EndFocus->bAlwaysRelevant = true;
			EndFocus->SetReplicates(true);
			EndFocus->ForceNetUpdate();
			EndGameFocus = EndFocus;
		}
		else
		{
			EndGameFocus = WinningBase;
		}
	}	

	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		AMultiTeamPlayerController* Controller = Cast<AMultiTeamPlayerController>(*Iterator);
		if (Controller && Controller->UTPlayerState && Controller->UTPlayerState->Team)
		{
			Controller->GameHasEnded(EndGameFocus, (Controller->UTPlayerState->Team->TeamIndex == WinningTeam->GetTeamNum()));
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
}

#if !UE_SERVER
void AUTDomGameMode::CreateConfigWidgets(TSharedPtr<class SVerticalBox> MenuSpace, bool bCreateReadOnly, TArray< TSharedPtr<TAttributePropertyBase> >& ConfigProps, int32 MinimumPlayers)
{
	Super::CreateConfigWidgets(MenuSpace, bCreateReadOnly, ConfigProps, MinimumPlayers);

	TSharedPtr< TAttributePropertyBool > AllowTransAttr = StaticCastSharedPtr<TAttributePropertyBool>(FindGameURLOption(ConfigProps, TEXT("AllowTrans")));

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

	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTDomGameMode", "Score", "Player Score"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float { return PS->Score;	})), StatList);

	LeftPane->AddSlot().AutoHeight()[SNew(SBox).HeightOverride(40.0f)];
	LeftPane->AddSlot().AutoHeight()[SNew(SBox)
		.HeightOverride(50.0f)
		[
			SNew(STextBlock)
			.TextStyle(SUWindowsStyle::Get(), "UT.Common.BoldText")
		.Text(NSLOCTEXT("AUTDomGameMode", "Scoring", " SCORING "))
		]
	];

	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTDomGameMode", "RegularKillPoints", "Score from Frags"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float { return PS->GetStatsValue(NAME_RegularKillPoints);	})), StatList);
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
		.Text(NSLOCTEXT("AUTDomGameMode", "ControlPointStats", " CONTROL POINTS STATS "))
		]
	];

	TAttributeStat::StatValueTextFunc ToTime = [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> FText
	{
		int32 Seconds = (int32)Stat->GetValue();
		int32 Mins = Seconds / 60;
		Seconds -= Mins * 60;
		return FText::FromString(FString::Printf(TEXT("%d:%02d"), Mins, Seconds));
	};

	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTDomGameMode", "ControlPointHeldPoints", "Points from Capture"), MakeShareable(new TAttributeStat(PlayerState, NAME_ControlPointHeldPoints)), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTDomGameMode", "ControlPointCaps", "Captures"), MakeShareable(new TAttributeStat(PlayerState, NAME_ControlPointCaps)), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTDomGameMode", "ControlPointHeldTime", "Total Held Time"), MakeShareable(new TAttributeStat(PlayerState, NAME_ControlPointHeldTime, nullptr, ToTime)), StatList);
}

#endif
