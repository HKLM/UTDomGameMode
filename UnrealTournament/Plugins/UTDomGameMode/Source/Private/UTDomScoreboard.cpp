// Created by Brian 'Snake' Alexander, 2015

#include "UnrealTournament.h"
#include "UTTeamScoreboard.h"
#include "UTDomGameState.h"
#include "UTDemoRecSpectator.h"
#include "StatNames.h"
#include "UTDomStat.h"
#include "UTDomScoreboard.h"

UUTDomScoreboard::UUTDomScoreboard(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NumPages = 2;
}

void UUTDomScoreboard::Draw_Implementation(float RenderDelta)
{
	Super::UUTHUDWidget::Draw_Implementation(RenderDelta);

	float YOffset = 64.f;
	DrawGamePanel(RenderDelta, YOffset);
	if (UTHUDOwner->ScoreboardPage > 0)
	{
		DrawScoringStats(RenderDelta, YOffset);
	}
	else
	{
		DrawTeamPanel(RenderDelta, YOffset);
		DrawScorePanel(RenderDelta, YOffset);
	}
	DrawServerPanel(RenderDelta, FooterPosY);
}

void UUTDomScoreboard::DrawScorePanel(float RenderDelta, float& YOffset)
{
	if (bIsInteractive)
	{
		SelectionStack.Empty();
	}
	if (UTGameState)
	{
		float DrawY = YOffset;
		DrawScoreHeaders(RenderDelta, DrawY);
		DrawPlayerScores(RenderDelta, DrawY);
	}
}

void UUTDomScoreboard::DrawGamePanel(float RenderDelta, float& YOffset)
{
	// Draw the Background
	DrawTexture(TextureAtlas, 0, 0, Size.X, 128, 4, 2, 124, 128, 1.0);

	// Draw the Logo
	DrawTexture(TextureAtlas, 165, 60, 301, 98, 162, 14, 301, 98.0, 1.0f, FLinearColor::White, FVector2D(0.5, 0.5));

	// Draw the Spacer Bar
	DrawTexture(TextureAtlas, 325, 60, 4, 99, 488, 13, 4, 99, 1.0f, FLinearColor::White, FVector2D(0.0, 0.5));
	FText MapName = UTHUDOwner ? FText::FromString(UTHUDOwner->GetWorld()->GetMapName().ToUpper()) : FText::GetEmpty();
	FText GameName = FText::GetEmpty();
	if (UTGameState && UTGameState->GameModeClass)
	{
		AUTGameMode* DefaultGame = UTGameState->GameModeClass->GetDefaultObject<AUTGameMode>();
		if (DefaultGame)
		{
			AUTDomGameState* GS = Cast<AUTDomGameState>(UTGameState);
			if (GameName.IsEmpty())
			{
				GameName = FText::FromString(DefaultGame->DisplayName.ToString().ToUpper());
			}
		}
	}

	if (!GameName.IsEmpty())
	{
		DrawText(GameName, 355, 28, UTHUDOwner->LargeFont, 1.0, 1.0, FLinearColor::White, ETextHorzPos::Left, ETextVertPos::Center); // 470
	}
	if (!MapName.IsEmpty())
	{
		DrawText(MapName, 355, 80, UTHUDOwner->LargeFont, 1.0, 1.0, FLinearColor::White, ETextHorzPos::Left, ETextVertPos::Center); // 470
	}

	DrawGameOptions(RenderDelta, YOffset);
	YOffset += 128;	// The size of this zone.
}

void UUTDomScoreboard::DrawGameOptions(float RenderDelta, float& YOffset)
{
	if (UTGameState)
	{
		if (UTGameState->GoalScore > 0)
		{
			// Draw Game Text
			FText Score = FText::Format(NSLOCTEXT("UTDomScoreboard", "GoalScoreFormat", "First Team to {0} Points"), FText::AsNumber(UTGameState->GoalScore));
			DrawText(Score, Size.X * 0.985, 28, UTHUDOwner->MediumFont, 1.0, 1.0, FLinearColor::White, ETextHorzPos::Right, ETextVertPos::Center);
		}
		else if (UTGameState->GoalScore == 0)
		{
			// Draw Game Text
			FText Score = FText::Format(NSLOCTEXT("UTDomScoreboard", "GoalTimeFormat", "Score most points in {0} minutes"), FText::AsNumber(UTGameState->TimeLimit));
			DrawText(Score, Size.X * 0.985, 28, UTHUDOwner->MediumFont, 1.0, 1.0, FLinearColor::White, ETextHorzPos::Right, ETextVertPos::Center);
		}

		FText StatusText = UTGameState->GetGameStatusText();
		if (!StatusText.IsEmpty())
		{
			DrawText(StatusText, Size.X * 0.985, 90, UTHUDOwner->MediumFont, 1.0, 1.0, FLinearColor::Yellow, ETextHorzPos::Right, ETextVertPos::Center);
		}
		else
		{
			FText Timer = (UTGameState->TimeLimit > 0) ? UTHUDOwner->ConvertTime(FText::GetEmpty(), FText::GetEmpty(), UTGameState->RemainingTime, true, true, true) :
				UTHUDOwner->ConvertTime(FText::GetEmpty(), FText::GetEmpty(), UTGameState->ElapsedTime, true, true, true);
			DrawText(Timer, Size.X * 0.985, 88, UTHUDOwner->NumberFont, 1.0, 1.0, FLinearColor::White, ETextHorzPos::Right, ETextVertPos::Center);
		}
	}
}

void UUTDomScoreboard::DrawTeamPanel(float RenderDelta, float& YOffset)
{
	if (UTGameState == NULL || UTGameState->Teams.Num() < 2 || UTGameState->Teams[0] == NULL || UTGameState->Teams[1] == NULL) return;

	// Draw the front end End
	float Width = (Size.X * 0.5) - CenterBuffer;

	float FrontSize = 35;
	float EndSize = 16;
	float MiddleSize = Width - FrontSize - EndSize;
	float XOffset = 0;

	// Draw the Background
	//RED
	DrawTexture(TextureAtlas, XOffset, YOffset + 22, 35, 65, 0, 188, 36, 65, 1.0, FLinearColor::Red);
	DrawTexture(TextureAtlas, XOffset + FrontSize, YOffset + 22, MiddleSize, 65, 39, 188, 64, 65, 1.0, FLinearColor::Red);
	DrawTexture(TextureAtlas, XOffset + FrontSize + MiddleSize, YOffset + 22, EndSize, 65, 39, 188, 64, 65, 1.0, FLinearColor::Red);

	DrawText(NSLOCTEXT("UTTeamScoreboard", "RedTeam", "RED"), 36, YOffset  + CellHeight, UTHUDOwner->HugeFont, 1.0, 1.0, FLinearColor::White, ETextHorzPos::Left, ETextVertPos::Center);
	DrawText(FText::AsNumber(UTGameState->Teams[0]->Score), Width * 0.9, YOffset + 48, UTHUDOwner->ScoreFont, false, FVector2D(0, 0), FLinearColor::Black, true, FLinearColor::Black, 0.75, 1.0, FLinearColor::White, ETextHorzPos::Right, ETextVertPos::Center);

	XOffset = Size.X - Width;

	//BLUE
	DrawTexture(TextureAtlas, XOffset + EndSize + MiddleSize, YOffset + 22, 35, 65, 196, 188, 36, 65, 1.0, FLinearColor::Blue);
	DrawTexture(TextureAtlas, XOffset + EndSize, YOffset + 22, MiddleSize, 65, 130, 188, 64, 65, 1.0, FLinearColor::Blue);
	DrawTexture(TextureAtlas, XOffset, YOffset + 22, EndSize, 65, 117, 188, 16, 65, 1.0, FLinearColor::Blue);

	DrawText(NSLOCTEXT("UTTeamScoreboard", "BlueTeam", "BLUE"), 1237, YOffset + CellHeight, UTHUDOwner->HugeFont, 1.0, 1.0, FLinearColor::White, ETextHorzPos::Right, ETextVertPos::Center);
	DrawText(FText::AsNumber(UTGameState->Teams[1]->Score), Size.X - Width + (Width * 0.1), YOffset + 48, UTHUDOwner->ScoreFont, false, FVector2D(0, 0), FLinearColor::Black, true, FLinearColor::Black, 0.75, 1.0, FLinearColor::White, ETextHorzPos::Left, ETextVertPos::Center);
	YOffset += 119;
}

void UUTDomScoreboard::DrawPlayerScores(float RenderDelta, float& YOffset)
{
	if (UTGameState == nullptr)
	{
		return;
	}

	int32 NumSpectators = 0;
	int32 XOffset = 0;
	AUTDomGameState* GS = Cast<AUTDomGameState>(UTGameState);

	for (int8 Team = 0; Team < 2; Team++)
	{
		int32 Place = 1;
		float DrawOffset = YOffset;
		for (int32 i = 0; i < UTGameState->PlayerArray.Num(); i++)
		{
			AUTPlayerState* PlayerState = Cast<AUTPlayerState>(UTGameState->PlayerArray[i]);
			if (PlayerState)
			{
				if (!PlayerState->bOnlySpectator)
				{
					if (PlayerState->GetTeamNum() == Team)
					{
						DrawPlayer(Place, PlayerState, RenderDelta, XOffset, DrawOffset);
						Place++;
						DrawOffset += CellHeight;
					}
				}
				else if (Team == 0 && (Cast<AUTDemoRecSpectator>(UTPlayerOwner) == nullptr && !PlayerState->bIsDemoRecording))
				{
					NumSpectators++;
				}
			}
		}
		XOffset = Size.X - ((Size.X * 0.5) - CenterBuffer);
	}

	if (UTGameState->PlayerArray.Num() <= 28 && NumSpectators > 0)
	{
		FText SpectatorCount = (NumSpectators == 1)
			? NSLOCTEXT("UTScoreboard", "OneSpectator", "1 spectator is watching this match")
			: FText::Format(NSLOCTEXT("UTScoreboard", "SpectatorFormat", "{0} spectators are watching this match"), FText::AsNumber(NumSpectators));
		DrawText(SpectatorCount, Size.X * 0.5, 765, UTHUDOwner->SmallFont, 1.0f, 1.0f, FLinearColor(0.75f, 0.75f, 0.75f, 1.0f), ETextHorzPos::Center, ETextVertPos::Bottom);
	}
}

void UUTDomScoreboard::DrawPlayerStats(AUTPlayerState* PS, float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float MaxHeight, const FStatsFontInfo& StatsFontInfo)
{
	FStatsFontInfo TinyFontInfo;
	TinyFontInfo.TextRenderInfo.bEnableShadow = true;
	TinyFontInfo.TextRenderInfo.bClipText = true;
	TinyFontInfo.TextFont = UTHUDOwner->TinyFont;
	float XL, SmallYL;
	Canvas->TextSize(UTHUDOwner->TinyFont, "TEST", XL, SmallYL, RenderScale, RenderScale);
	TinyFontInfo.TextHeight = SmallYL;

	Canvas->DrawText(UTHUDOwner->TinyFont, TEXT("Amount"), XOffset + (ValueColumn - 0.025f)*ScoreWidth, YPos, RenderScale, RenderScale, StatsFontInfo.TextRenderInfo);
	Canvas->DrawText(UTHUDOwner->TinyFont, TEXT("Scoring"), XOffset + (ScoreColumn - 0.025f)*ScoreWidth, YPos, RenderScale, RenderScale, StatsFontInfo.TextRenderInfo);
	YPos += 0.5f * StatsFontInfo.TextHeight;
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "Kills", "Kills"), PS->Kills, -1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	int32 ControlPointHeldPt = PS->GetStatsValue(NAME_ControlPointHeldPoints);
	int32 RegularKills = PS->Kills - ControlPointHeldPt;
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "RegKills", " - Regular Kills"), RegularKills, PS->GetStatsValue(NAME_RegularKillPoints), DeltaTime, XOffset, YPos, TinyFontInfo, ScoreWidth);
	DrawStatsLine(NSLOCTEXT("UTDomScoreboard", "ControlPointHeldPoints", " - Control Point Score"), ControlPointHeldPt, PS->GetStatsValue(NAME_ControlPointHeldPoints), DeltaTime, XOffset, YPos, TinyFontInfo, ScoreWidth);
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "Deaths", "Deaths"), PS->Deaths, -1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	DrawTextStatsLine(NSLOCTEXT("UTScoreboard", "ScorePM", "Score Per Minute"), FString::Printf(TEXT(" %6.2f"), ((PS->StartTime <  GetWorld()->GameState->ElapsedTime) ? PS->Score * 60.f / (GetWorld()->GameState->ElapsedTime - PS->StartTime) : 0.f)), "", DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	Canvas->DrawText(StatsFontInfo.TextFont, "----------------------------------------------------------------", XOffset, YPos, RenderScale, RenderScale, StatsFontInfo.TextRenderInfo);
	YPos += 0.5f * StatsFontInfo.TextHeight;

	DrawStatsLine(NSLOCTEXT("UTDomScoreboard", "ControlPointCap", "Control Point Captures"), PS->GetStatsValue(NAME_ControlPointCaps), -1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	FText ClockString = UTHUDOwner->ConvertTime(FText::GetEmpty(), FText::GetEmpty(), PS->GetStatsValue(NAME_ControlPointHeldTime), false);
	DrawTextStatsLine(NSLOCTEXT("UTDomScoreboard", "ControlPointHeldTime", "Control Point Held Time"), ClockString.ToString(), "", DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);

	Canvas->DrawText(UTHUDOwner->TinyFont, TEXT("-----"), XOffset + (ScoreColumn - 0.015f)*ScoreWidth, YPos, RenderScale, RenderScale, StatsFontInfo.TextRenderInfo);
	YPos += 0.5f*StatsFontInfo.TextHeight;
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "Scoring", "TOTAL SCORE"), -1, PS->Score, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
}

void UUTDomScoreboard::DrawTeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo)
{
	// draw team stats
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "TeamKills", "Kills"), UTGameState->Teams[0]->GetStatsValue(NAME_TeamKills), UTGameState->Teams[1]->GetStatsValue(NAME_TeamKills), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);

	// find top scorer
	AUTPlayerState* TopScorerRed = FindTopTeamScoreFor(0);
	AUTPlayerState* TopScorerBlue = FindTopTeamScoreFor(1);
	AUTPlayerState* TopTeamCapRed = FindTopTeamScoreFor(0);
	AUTPlayerState* TopTeamCapBlue = FindTopTeamScoreFor(1);

	// find top kills && KD
	AUTPlayerState* TopKillerRed = FindTopTeamKillerFor(0);
	AUTPlayerState* TopKillerBlue = FindTopTeamKillerFor(1);
	AUTPlayerState* TopKDRed = FindTopTeamKDFor(0);
	AUTPlayerState* TopKDBlue = FindTopTeamKDFor(1);
	AUTPlayerState* TopSPMRed = FindTopTeamSPMFor(0);
	AUTPlayerState* TopSPMBlue = FindTopTeamSPMFor(1);

	DrawTextStatsLine(NSLOCTEXT("UTScoreboard", "TopScorer", "Top Scorer"), GetPlayerNameFor(TopScorerRed), GetPlayerNameFor(TopScorerBlue), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	DrawTextStatsLine(NSLOCTEXT("UTDomScoreboard", "TopCaps", "Top Conrtol Point Captures"), GetPlayerNameFor(TopTeamCapRed), GetPlayerNameFor(TopTeamCapBlue), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);

	DrawTextStatsLine(NSLOCTEXT("UTScoreboard", "TopKills", "Top Kills"), GetPlayerNameFor(TopKillerRed), GetPlayerNameFor(TopKillerBlue), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	DrawTextStatsLine(NSLOCTEXT("UTScoreboard", "TopKD", "Top K/D"), GetPlayerNameFor(TopKDRed), GetPlayerNameFor(TopKDBlue), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	DrawTextStatsLine(NSLOCTEXT("UTScoreboard", "TopSPM", "Top Score Per Minute"), GetPlayerNameFor(TopSPMRed), GetPlayerNameFor(TopSPMBlue), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);

	DrawStatsLine(NSLOCTEXT("UTScoreboard", "BeltPickups", "Shield Belt Pickups"), UTGameState->Teams[0]->GetStatsValue(NAME_ShieldBeltCount), UTGameState->Teams[1]->GetStatsValue(NAME_ShieldBeltCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "VestPickups", "Armor Vest Pickups"), UTGameState->Teams[0]->GetStatsValue(NAME_ArmorVestCount), UTGameState->Teams[1]->GetStatsValue(NAME_ArmorVestCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "PadPickups", "Thigh Pad Pickups"), UTGameState->Teams[0]->GetStatsValue(NAME_ArmorPadsCount), UTGameState->Teams[1]->GetStatsValue(NAME_ArmorPadsCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "HelmetPickups", "Helmet Pickups"), UTGameState->Teams[0]->GetStatsValue(NAME_HelmetCount), UTGameState->Teams[1]->GetStatsValue(NAME_HelmetCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);

	int32 TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_UDamageCount);
	int32 TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_UDamageCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "UDamagePickups", "UDamage Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
	TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_BerserkCount);
	TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_BerserkCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "BerserkPickups", "Berserk Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
	TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_InvisibilityCount);
	TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_InvisibilityCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "InvisibilityPickups", "Invisibility Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
	TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_KegCount);
	TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_KegCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "KegPickups", "Keg Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}

	DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "UDamage", "UDamage Control"), NAME_UDamageTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);
	DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "Berserk", "Berserk Control"), NAME_BerserkTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);
	DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "Invisibility", "Invisibility Control"), NAME_InvisibilityTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);

	int32 BootJumpsRed = UTGameState->Teams[0]->GetStatsValue(NAME_BootJumps);
	int32 BootJumpsBlue = UTGameState->Teams[1]->GetStatsValue(NAME_BootJumps);
	if ((BootJumpsRed != 0) || (BootJumpsBlue != 0))
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "JumpBootJumps", "JumpBoot Jumps"), BootJumpsRed, BootJumpsBlue, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
}
