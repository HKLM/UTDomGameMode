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
	PlayerStateClass = AUTDomPlayerState::StaticClass();
	PlayerControllerClass = AUTDomPlayerController::StaticClass();
	VictoryMessageClass = UUTDomVictoryMessage::StaticClass();
	MapPrefix = TEXT("DOM");
	bAllowOvertime = false;
	bUseTeamStarts = false;

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
	TeamColors[2] = FLinearColor(0.0f, 0.75f, 0.0f, 1.0f); 
 	TeamColors[3] = FLinearColor(0.65f, 0.65f, 0.0f, 1.0f); 

	TeamSkins.InsertUninitialized(0, 1);
	TeamSkins.InsertUninitialized(1, 1);
	TeamSkins.InsertUninitialized(2, 1);
	TeamSkins.InsertUninitialized(3, 1);

	TeamSkins[0].TeamBodyColor = FLinearColor(5.0f, 0.1f, 0.0f, 1.0f);
	TeamSkins[0].TeamBodyOverlay = FLinearColor(10.0f, 0.1f, 0.0f, 1.0f);
	TeamSkins[1].TeamBodyColor = FLinearColor(0.0f, 0.0f, 5.18f, 1.0f);
	TeamSkins[1].TeamBodyOverlay =FLinearColor(0.04f, 0.04f, 10.4f, 1.0f);
	TeamSkins[2].TeamBodyColor = FLinearColor(0.1f, 5.0f, 0.0f, 1.0f);
	TeamSkins[2].TeamBodyOverlay = FLinearColor(0.1f, 10.0f, 0.0f, 1.0f);
	TeamSkins[3].TeamBodyColor = FLinearColor(2.1f, 2.1f, 0.0f, 1.0f);
	TeamSkins[3].TeamBodyOverlay = FLinearColor(4.1f, 4.1f, 0.0f, 1.0f);
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

void AUTDomGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);
	if (NumTeams > 2)
	{
		AUTPlayerState* UTPS = Cast<AUTPlayerState>(PlayerPawn->PlayerState);
		Set4TeamSkinForCharacter(UTPS);
	}
}

bool AUTDomGameMode::MovePlayerToTeam(AController* Player, AUTPlayerState* PS, uint8 NewTeam)
{
	if (Teams.IsValidIndex(NewTeam) && (PS->Team == NULL || PS->Team->TeamIndex != NewTeam))
	{
		//Make sure we kill the player before they switch sides so the correct team loses the point
		AUTCharacter* UTC = Cast<AUTCharacter>(Player->GetPawn());
		if (UTC != nullptr)
		{
			UTC->PlayerSuicide();
		}

		if (PS->Team != NULL)
		{
			PS->Team->RemoveFromTeam(Player);
		}
		Teams[NewTeam]->AddToTeam(Player);
		PS->bPendingTeamSwitch = false;
		PS->ForceNetUpdate();
		if (NumTeams > 2)
		{
			Set4TeamSkinForCharacter(PS);
		}

		// Clear the player's gameplay mute list.
		APlayerController* PlayerController = Cast<APlayerController>(Player);
		AUTGameState* MyGameState = GetWorld()->GetGameState<AUTGameState>();

		if (PlayerController && MyGameState)
		{
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				AUTPlayerController* NextPlayer = Cast<AUTPlayerController>(*Iterator);
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

bool AUTDomGameMode::Set4TeamSkinForCharacter(AUTPlayerState* PS)
{
	if (PS == NULL || PS->Team == NULL)
	{
		return false;
	}
	float FTeamNum;
	if (PS->GetTeamNum() > 1 && PS->GetTeamNum() != 255) //Green & Gold
	{
		FTeamNum = (PS->GetTeamNum() == 2) ? 1.0f : 0.0f;
	}
	else
	{
		FTeamNum = PS->GetTeamNum();
	}

	AUTCharacter* UTC = PS->GetUTCharacter();
	if (UTC != NULL)
	{
		FLinearColor PlayerTeamColor = PS->Team->TeamColor;
		int32 i = 0;
		for (UMaterialInstanceDynamic* MIDom : UTC->GetBodyMIs())
		{
			if (MIDom != NULL)
			{
				i++;
				static FName NAME_TeamRedColor(TEXT("Red Team Color"));
				static FName NAME_TeamBlueColor(TEXT("Blue Team Color"));
				MIDom->SetScalarParameterValue("Use Team Colors", 1.0f);
				if (FTeamNum != 255)
				{
					MIDom->SetScalarParameterValue("TeamSelect", FTeamNum);
					if (PS->GetTeamNum() == 1 || PS->GetTeamNum() == 2) //Blue & Green Team
					{
						static FName NAME_TeamBlueOverlay(TEXT("Blue Overlay"));
						MIDom->SetVectorParameterValue(NAME_TeamBlueColor, TeamSkins[PS->GetTeamNum()].TeamBodyColor);
						MIDom->SetVectorParameterValue(NAME_TeamBlueOverlay, TeamSkins[PS->GetTeamNum()].TeamBodyOverlay);
						//merc
						static FName NAME_MercMaleTeamBluePlastic(TEXT("Blue Team Plastic"));
						MIDom->SetVectorParameterValue(NAME_MercMaleTeamBluePlastic,  TeamSkins[PS->GetTeamNum()].TeamBodyColor);
						static FName NAME_MercMaleTeamBlueCamo(TEXT("Blue Team Camo"));
						MIDom->SetVectorParameterValue(NAME_MercMaleTeamBlueCamo, PlayerTeamColor);
						static FName NAME_MercMaleTeamBlueNylon(TEXT("Blue Team Nylon"));
						MIDom->SetVectorParameterValue(NAME_MercMaleTeamBlueNylon, PlayerTeamColor);
						//nec male
						static FName NAME_NecMaleTeamBlueDarkPlastic(TEXT("Blue Team Dark Plastic"));
						MIDom->SetVectorParameterValue(NAME_NecMaleTeamBlueDarkPlastic, PlayerTeamColor);
						static FName NAME_NecMaleTeamBlueLatex(TEXT("Blue Team Latex"));
						MIDom->SetVectorParameterValue(NAME_NecMaleTeamBlueLatex, PlayerTeamColor);
						//visse
						static FName NAME_VisseTeamBlueRubberTint(TEXT("Rubber Tint"));
						MIDom->SetVectorParameterValue(NAME_VisseTeamBlueRubberTint, PlayerTeamColor);
						//Skaarj
						static FName NAME_SkaarjTeamBlueTintAlpha(TEXT("Blue tint on alpha"));
						MIDom->SetVectorParameterValue(NAME_SkaarjTeamBlueTintAlpha, PlayerTeamColor);
					}
					else if (UTC->GetTeamNum() == 0 || UTC->GetTeamNum() == 3) //Red & Gold Team
					{
						static FName NAME_TeamRedOverlay(TEXT("Red Overlay"));
						MIDom->SetVectorParameterValue(NAME_TeamRedColor, TeamSkins[PS->GetTeamNum()].TeamBodyColor);
						MIDom->SetVectorParameterValue(NAME_TeamRedOverlay, TeamSkins[PS->GetTeamNum()].TeamBodyOverlay);
						//merc
						static FName NAME_MercMaleTeamRedPlastic(TEXT("Red Team Plastic"));
						MIDom->SetVectorParameterValue(NAME_MercMaleTeamRedPlastic, TeamSkins[PS->GetTeamNum()].TeamBodyColor);
						static FName NAME_MercMaleTeamRedCamo(TEXT("Red Team Camo"));
						MIDom->SetVectorParameterValue(NAME_MercMaleTeamRedCamo, PlayerTeamColor);
						static FName NAME_MercMaleTeamRedNylon(TEXT("Red Team Nylon"));
						MIDom->SetVectorParameterValue(NAME_MercMaleTeamRedNylon, PlayerTeamColor);
						//nec male
						static FName NAME_NecMaleTeamRedDarkPlastic(TEXT("Red Team Dark Plastic"));
						MIDom->SetVectorParameterValue(NAME_NecMaleTeamRedDarkPlastic, PlayerTeamColor);
						static FName NAME_NecMaleTeamRedLatex(TEXT("Red Team Latex"));
						MIDom->SetVectorParameterValue(NAME_NecMaleTeamRedLatex, PlayerTeamColor);
						//visse
						static FName NAME_VisseTeamRedRubberTint(TEXT("Rubber Tint"));
						MIDom->SetVectorParameterValue(NAME_VisseTeamRedRubberTint, PlayerTeamColor);
						//Skaarj
						static FName NAME_SkaarjTeamRedTintAlpha(TEXT("Red tint on alpha"));
						MIDom->SetVectorParameterValue(NAME_SkaarjTeamRedTintAlpha, PlayerTeamColor);

					//	static FName NAME_TeamBlueOverlay(TEXT("Blue Overlay"));
					//	MIDom->SetVectorParameterValue(NAME_TeamBlueColor, PlayerTeamColor);/* FLinearColor(0.0, 0.0, 5.18, 1.0));*/
					//	MIDom->SetVectorParameterValue(NAME_TeamBlueOverlay, PlayerTeamColor);/* FLinearColor(0.04, 0.04, 10.4, 1.0));*/
					//	//merc
					//	static FName NAME_MercMaleTeamBluePlastic(TEXT("Blue Team Plastic"));
					//	MIDom->SetVectorParameterValue(NAME_MercMaleTeamBluePlastic, PlayerTeamColor);
					//	static FName NAME_MercMaleTeamBlueCamo(TEXT("Blue Team Camo"));
					//	MIDom->SetVectorParameterValue(NAME_MercMaleTeamBlueCamo, PlayerTeamColor);
					//	static FName NAME_MercMaleTeamBlueNylon(TEXT("Blue Team Nylon"));
					//	MIDom->SetVectorParameterValue(NAME_MercMaleTeamBlueNylon, PlayerTeamColor);
					//	//nec male
					//	static FName NAME_NecMaleTeamBlueDarkPlastic(TEXT("Blue Team Dark Plastic"));
					//	MIDom->SetVectorParameterValue(NAME_NecMaleTeamBlueDarkPlastic, PlayerTeamColor);
					//	static FName NAME_NecMaleTeamBlueLatex(TEXT("Blue Team Latex"));
					//	MIDom->SetVectorParameterValue(NAME_NecMaleTeamBlueLatex, PlayerTeamColor);
					//	//visse
					//	static FName NAME_VisseTeamBlueRubberTint(TEXT("Rubber Tint"));
					//	MIDom->SetVectorParameterValue(NAME_VisseTeamBlueRubberTint, PlayerTeamColor);
					//	//Skaarj
					//	static FName NAME_SkaarjTeamBlueTintAlpha(TEXT("Blue tint on alpha"));
					//	MIDom->SetVectorParameterValue(NAME_SkaarjTeamBlueTintAlpha, PlayerTeamColor);
					//}
					//else if (UTC->GetTeamNum() == 0 || UTC->GetTeamNum() == 3) //Red & Gold Team
					//{
					//	static FName NAME_TeamRedOverlay(TEXT("Red Overlay"));
					//	MIDom->SetVectorParameterValue(NAME_TeamRedColor, PlayerTeamColor);/* FLinearColor(0.1, 5.0, 0.0, 1.0));*/
					//	MIDom->SetVectorParameterValue(NAME_TeamRedOverlay, PlayerTeamColor);/* FLinearColor(0.1, 10.0, 0.0, 1.0));*/
					//	//merc
					//	static FName NAME_MercMaleTeamRedPlastic(TEXT("Red Team Plastic"));
					//	MIDom->SetVectorParameterValue(NAME_MercMaleTeamRedPlastic, PlayerTeamColor);
					//	static FName NAME_MercMaleTeamRedCamo(TEXT("Red Team Camo"));
					//	MIDom->SetVectorParameterValue(NAME_MercMaleTeamRedCamo, PlayerTeamColor);
					//	static FName NAME_MercMaleTeamRedNylon(TEXT("Red Team Nylon"));
					//	MIDom->SetVectorParameterValue(NAME_MercMaleTeamRedNylon, PlayerTeamColor);
					//	//nec male
					//	static FName NAME_NecMaleTeamRedDarkPlastic(TEXT("Red Team Dark Plastic"));
					//	MIDom->SetVectorParameterValue(NAME_NecMaleTeamRedDarkPlastic, PlayerTeamColor);
					//	static FName NAME_NecMaleTeamRedLatex(TEXT("Red Team Latex"));
					//	MIDom->SetVectorParameterValue(NAME_NecMaleTeamRedLatex, PlayerTeamColor);
					//	//visse
					//	static FName NAME_VisseTeamRedRubberTint(TEXT("Rubber Tint"));
					//	MIDom->SetVectorParameterValue(NAME_VisseTeamRedRubberTint, PlayerTeamColor);
					//	//Skaarj
					//	static FName NAME_SkaarjTeamRedTintAlpha(TEXT("Red tint on alpha"));
					//	MIDom->SetVectorParameterValue(NAME_SkaarjTeamRedTintAlpha, PlayerTeamColor);
					}
				}
				else
				{
					MIDom->SetScalarParameterValue("TeamSelect", UTC->GetTeamNum());
				}
				UTC->UpdateSkin();
			}
		}
		if (i != 0)
		{
			UTC->ForceNetUpdate();
			return true;
		}
	}
	return false;
}

bool AUTDomGameMode::CheckScore_Implementation(AUTPlayerState* Scorer)
{
	AUTDomTeamInfo* WinningTeam = NULL;
	AUTPlayerState* BestPlayer = Scorer;
	// check if team wins by points
	if (GoalScore > 0)
	{
		for (uint8 i = 0; i < NumTeams; i++)
		{
			if (Teams.IsValidIndex(i) 
				&& Cast<AUTDomTeamInfo>(Teams[i]) != NULL
				&& Cast<AUTDomTeamInfo>(Teams[i])->GetFloatScore() >= GoalScore)
			{
				BestPlayer = FindBestPlayerOnTeam(i);
				if (BestPlayer == NULL && Scorer->GetTeamNum() == i) BestPlayer = Scorer;
				//DomGameState->SetWinner(BestPlayer);
				EndGame(BestPlayer, FName(TEXT("scorelimit")));
				return true;
			}
		}
	}
	// check if team wins by time limit
	if (TimeLimit > 0 && DomGameState->GetRemainingTime() <= 0)
	{
		AUTDomTeamInfo* LeadingTeam = DomGameState->FindLeadingTeam();
		if (LeadingTeam != NULL)
		{
			BestPlayer = FindBestPlayerOnTeam(LeadingTeam->GetTeamNum());
			if (BestPlayer == NULL && Scorer->GetTeamNum() == LeadingTeam->GetTeamNum()) BestPlayer = Scorer;
			//DomGameState->SetWinner(BestPlayer);
			EndGame(BestPlayer, FName(TEXT("TimeLimit")));
			return true;
		}
	}
	return false;
}

// Use Team skins for 3/4 team play
void AUTDomGameMode::RestartPlayer(AController* aPlayer)
{
	Super::RestartPlayer(aPlayer);
	if ((aPlayer == NULL) || (aPlayer->PlayerState == NULL) || aPlayer->PlayerState->PlayerName.IsEmpty() || !IsMatchInProgress() || aPlayer->PlayerState->bOnlySpectator)
	{
		return;
	}
	//if (NumTeams > 2)
	//{
	//	if (Cast<AUTDomPlayerState>(aPlayer->PlayerState))
	//	{
	//		AUTDomPlayerState* PS = Cast<AUTDomPlayerState>(aPlayer->PlayerState);
	//		if (PS->Team && Cast<AUTDomTeamInfo>(PS->Team))
	//		{
	//			AUTDomTeamInfo* DTI = Cast<AUTDomTeamInfo>(PS->Team);
	//			if (DTI && DTI->TeamSkinOverlay && PS->GetUTCharacter())
	//			{
	//				AUTCharacter* UTC = PS->GetUTCharacter();
	//				UTC->SetCharacterOverlayEffect(FOverlayEffect(DTI->TeamSkinOverlay), true);
	//			}
	//		}
	//	}
	//}
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
	AControlPoint* WinningBase = NULL;
	if (Winner != NULL)
	{
		// find control point owned by winning player
		for (uint8 n = 0; n < DomGameState->GameControlPoints.Num(); n++)
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
			for (uint8 n = 0; n < DomGameState->GameControlPoints.Num(); n++)
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
			for (uint8 i = 0; i < DomGameState->GameControlPoints.Num(); i++)
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

void AUTDomGameMode::PlayEndOfMatchMessage()
{
	if (UTGameState && UTGameState->WinningTeam)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* Controller = Iterator->Get();
			if (Controller && Controller->IsA(AUTPlayerController::StaticClass()))
			{
				AUTPlayerController* PC = Cast<AUTPlayerController>(Controller);
				if (PC && Cast<AUTPlayerState>(PC->PlayerState))
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

	RightPane->AddSlot().AutoHeight()[SNew(SBox).HeightOverride(40.0f)];
	RightPane->AddSlot().AutoHeight()[SNew(SBox)
		.HeightOverride(50.0f)
		[
			SNew(STextBlock)
			.TextStyle(SUWindowsStyle::Get(), "UT.Common.BoldText")
		.Text(NSLOCTEXT("ADomination", "PickupStats", " PICKUP STATS "))
		]
	];

	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "BeltPickups", "Shield Belt Pickups"), MakeShareable(new TAttributeStat(PlayerState, NAME_ShieldBeltCount)), StatList);
	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "VestPickups", "Armor Vest Pickups"), MakeShareable(new TAttributeStat(PlayerState, NAME_ArmorVestCount)), StatList);
	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "PadPickups", "Thigh Pad Pickups"), MakeShareable(new TAttributeStat(PlayerState, NAME_ArmorPadsCount)), StatList);
	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "HelmetPickups", "Helmet Pickups"), MakeShareable(new TAttributeStat(PlayerState, NAME_HelmetCount)), StatList);
	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "JumpBootJumps", "JumpBoot Jumps"), MakeShareable(new TAttributeStat(PlayerState, NAME_BootJumps)), StatList);

	RightPane->AddSlot().AutoHeight()[SNew(SBox).HeightOverride(40.0f)];
	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "UDamagePickups", "UDamage Pickups"), MakeShareable(new TAttributeStat(PlayerState, NAME_UDamageCount)), StatList);
	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "BerserkPickups", "Berserk Pickups"), MakeShareable(new TAttributeStat(PlayerState, NAME_BerserkCount)), StatList);
	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "InvisibilityPickups", "Invisibility Pickups"), MakeShareable(new TAttributeStat(PlayerState, NAME_InvisibilityCount)), StatList);
	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "KegPickups", "Keg Pickups"), MakeShareable(new TAttributeStat(PlayerState, NAME_KegCount)), StatList);

	RightPane->AddSlot().AutoHeight()[SNew(SBox).HeightOverride(40.0f)];
	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "UDamageControl", "UDamage Control"), MakeShareable(new TAttributeStat(PlayerState, NAME_UDamageTime, nullptr, ToTime)), StatList);
	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "BerserkControl", "Berserk Control"), MakeShareable(new TAttributeStat(PlayerState, NAME_BerserkTime, nullptr, ToTime)), StatList);
	NewPlayerInfoLine(RightPane, NSLOCTEXT("AUTGameMode", "InvisibilityControl", "Invisibility Control"), MakeShareable(new TAttributeStat(PlayerState, NAME_InvisibilityTime, nullptr, ToTime)), StatList);
}

#endif
