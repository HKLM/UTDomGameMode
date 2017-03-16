// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTDoubleDomGameMode.h"
#include "UTDoubleDomGameState.h"
#include "UTDomGameMode.h"
#include "UTADomTypes.h"

#include "UTDomTeamInfo.h"
#include "UTDomSquadAI.h"
#include "ControlPoint.h"
#include "UTHUD_DOM.h"
#include "UTDoubleDomGameMessage.h"
#include "UTCountDownMessage.h"
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

#include "xDomPoint.h"
#include "xDomPointA.h"
#include "xDomPointB.h"

const int32 AUTDoubleDomGameMode::MAXTIMETOSCORE = 20;
const int32 AUTDoubleDomGameMode::MAXTIMEDISABLED = 20;

AUTDoubleDomGameMode::AUTDoubleDomGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AUTDoubleDomGameState::StaticClass();
	GameMessageClass = UUTDoubleDomGameMessage::StaticClass();
	bAllowOvertime = true;
	bUseTeamStarts = false;
	bNoTeamPlayerStart = true;
	TimeToScore = 10;
	TimeDisabled = 10;
	GoalScore = 5;
	MaxSquadSize = 2;
	MaxControlPoints = MAX_DOM_POINTS;
}

void AUTDoubleDomGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	bNoTeamPlayerStart = EvalBoolOptions(UGameplayStatics::ParseOption(Options, TEXT("bNoTeamPlayerStart")), bNoTeamPlayerStart);
	bBalanceTeams = EvalBoolOptions(UGameplayStatics::ParseOption(Options, TEXT("BalanceTeams")), bBalanceTeams);

	TimeToScore = FMath::Clamp(FMath::Min(UGameplayStatics::GetIntOption(Options, TEXT("TimeToScore"), TimeToScore), MAXTIMETOSCORE), 0, MAXTIMETOSCORE);
	TimeDisabled = FMath::Clamp(FMath::Min(UGameplayStatics::GetIntOption(Options, TEXT("TimeDisabled"), TimeDisabled), MAXTIMEDISABLED), 0, MAXTIMEDISABLED);
	MaxControlPoints = MAX_DOM_POINTS;
}

void AUTDoubleDomGameMode::InitGameState()
{
	Super::InitGameState();
	DomGameState = Cast<AUTDoubleDomGameState>(GameState);
}

void AUTDoubleDomGameMode::DefaultTimer()
{
	Super::AUTTeamGameMode::DefaultTimer();
	if (HasMatchStarted() && !HasMatchEnded())
	{
		if (!CDomPoints.IsValidIndex(0) || !CDomPoints.IsValidIndex(1) || (CDomPoints.IsValidIndex(0) && CDomPoints[0] == nullptr) || (CDomPoints.IsValidIndex(1) && CDomPoints[1] == nullptr))
		{
			return;
		}
		if (DisabledCountDown > 0)
		{
			// Play the "New Round In" announcement
			if (!bPlayedDisabledMsg && DisabledCountDown < 7)
			{
				DomGameState->SendDDomMessage(14, NULL);
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
				BroadcastLocalized( this, UUTCountDownMessage::StaticClass(), DisabledCountDown, NULL, NULL, NULL);
				//DDGS->SendDDomMessage(DisabledCountDown, NULL);
			}
		}
		// nothing more to do unless the same team controls both points
		if ((CDomPoints[0]->ControllingTeam == nullptr) || (CDomPoints[0]->ControllingTeam != CDomPoints[1]->ControllingTeam))
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
				DomGameState->SendDDomMessage(13, CDomPoints[0]->ControllingTeam);
				bPlayedTeamDominating = true;
			}
			else if (ScoreCountDown < 8) // Play the Count down announcements
			{
				BroadcastLocalized( this, UUTCountDownMessage::StaticClass(), ScoreCountDown, NULL, NULL, NULL);

				//DDGS->SendDDomMessage(ScoreCountDown, NULL);
			}

			// decrement the time remaining until a point is scored
			ScoreCountDown--;
			return;
		}
		// controlling team has scored!
		ScoreDDomTeam();

		// check if the game is over
		if (bAllowOvertime && DomGameState->GetRemainingTime() <= 0)
		{
			//TODO - add sudden death system? 
			SetMatchState(MatchState::MatchIsInOvertime);
		}
		else if (!bAllowOvertime && DomGameState->GetRemainingTime() <= 0)
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
	if (IsMatchInProgress() && !HasMatchEnded() && TimeLimit > 0 && DomGameState->GetRemainingTime() <= 0)
	{
		// Game should be over.. look to see if we need to go in to overtime....	
		bool bTied = false;
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
		if (CDomPoints[i]->ControllingPawn != nullptr)
		{
			CDomPoints[i]->ControllingPawn->Score += 5.f;
			CDomPoints[i]->UpdateHeldPointStat(CDomPoints[i]->ControllingPawn, 5);
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
	AUTTeamInfo* WinningTeam = nullptr;
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
	if (TimeLimit > 0 && DomGameState->GetRemainingTime() <= 0)
	{
		if (!bAllowOvertime)
		{
			AUTTeamInfo* LeadingTeam = DomGameState->FindLeadingTeam();
			if (LeadingTeam != nullptr)
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

void AUTDoubleDomGameMode::CreateGameURLOptions(TArray<TSharedPtr<TAttributePropertyBase>>& MenuProps)
{
	Super::CreateGameURLOptions(MenuProps);
	MenuProps.Add(MakeShareable(new TAttributeProperty<int32>(this, &TimeToScore, TEXT("TimeToScore"))));
}

#if !UE_SERVER
void AUTDoubleDomGameMode::CreateConfigWidgets(TSharedPtr<class SVerticalBox> MenuSpace, bool bCreateReadOnly, TArray< TSharedPtr<TAttributePropertyBase> >& ConfigProps, int32 MinimumPlayers)
{
	Super::CreateConfigWidgets(MenuSpace, bCreateReadOnly, ConfigProps, MinimumPlayers);

	TSharedPtr< TAttributeProperty<int32> > TimeToScoreAttr = StaticCastSharedPtr<TAttributeProperty<int32>>(FindGameURLOption(ConfigProps, TEXT("TimeToScore")));

	if (TimeToScoreAttr.IsValid())
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
					.Text(NSLOCTEXT("UTDoubleDomGameMode", "TimeToScore", "Time To Score"))
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
						.Text(TimeToScoreAttr.ToSharedRef(), &TAttributeProperty<int32>::GetAsText)
					) :
					StaticCastSharedRef<SWidget>(
						SNew(SNumericEntryBox<int32>)
						.Value(TimeToScoreAttr.ToSharedRef(), &TAttributeProperty<int32>::GetOptional)
						.OnValueChanged(TimeToScoreAttr.ToSharedRef(), &TAttributeProperty<int32>::Set)
						.AllowSpin(true)
						.Delta(1)
						.MinValue(3)
						.MaxValue(20)
						.MinSliderValue(3)
						.MaxSliderValue(20)
						.EditableTextBoxStyle(SUWindowsStyle::Get(), "UT.Common.NumEditbox.White")
					)
				]
			]
		];
	}
}

#endif
