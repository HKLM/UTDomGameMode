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
		if (UTGameState != NULL && UTGameState->Teams.Num() > 2)
		{
			YOffset += 220;
			DrawMultiTeamPanel(RenderDelta, YOffset);
			DrawMultiScorePanel(RenderDelta, YOffset);
		}
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

void UUTDomScoreboard::DrawMultiScorePanel(float RenderDelta, float& YOffset)
{
	float DrawY = YOffset;
	DrawScoreHeaders(RenderDelta, DrawY);
	DrawMultiPlayerScores(RenderDelta, DrawY);
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
			if (GS != NULL && GS->KingOfTheHill)
			{
				GameName = NSLOCTEXT("UTDomScoreboard", "KingOfHill", "KING OF THE HILL");
			}
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

void UUTDomScoreboard::DrawMultiTeamPanel(float RenderDelta, float& YOffset)
{
	if (UTGameState == NULL || UTGameState->Teams.Num() < 2 || UTGameState->Teams[0] == NULL || UTGameState->Teams[1] == NULL) return;

	// Draw the front end End
	float Width = (Size.X * 0.5) - CenterBuffer;

	float FrontSize = 35;
	float EndSize = 16;
	float MiddleSize = Width - FrontSize - EndSize;
	float XOffset = 0;

	if (UTGameState->Teams.Num() > 2)
	{
		if (UTGameState->Teams[2] == NULL) return;
		XOffset = 0;
		//GREEN
		DrawTexture(TextureAtlas, XOffset, YOffset + 22, 35, 65, 0, 188, 36, 65, 1.0, FLinearColor::Green);
		DrawTexture(TextureAtlas, XOffset + FrontSize, YOffset + 22, MiddleSize, 65, 39, 188, 64, 65, 1.0, FLinearColor::Green);
		DrawTexture(TextureAtlas, XOffset + FrontSize + MiddleSize, YOffset + 22, EndSize, 65, 39, 188, 64, 65, 1.0, FLinearColor::Green);

		DrawText(NSLOCTEXT("UTDomScoreboard", "GreenTeam", "GREEN"), 36, YOffset  + CellHeight, UTHUDOwner->HugeFont, 1.0, 1.0, FLinearColor::White, ETextHorzPos::Left, ETextVertPos::Center);
		DrawText(FText::AsNumber(UTGameState->Teams[2]->Score), Width * 0.9, YOffset + 48, UTHUDOwner->ScoreFont, false, FVector2D(0, 0), FLinearColor::Black, true, FLinearColor::Black, 0.75, 1.0, FLinearColor::White, ETextHorzPos::Right, ETextVertPos::Center);
	}

	if (UTGameState->Teams.Num() == 4)
	{
		if (UTGameState->Teams[3] == NULL) return;
		//GOLD
		XOffset = Size.X - Width;
		DrawTexture(TextureAtlas, XOffset + EndSize + MiddleSize, YOffset + 22, 35, 65, 196, 188, 36, 65, 1.0, FLinearColor::Yellow);
		DrawTexture(TextureAtlas, XOffset + EndSize, YOffset + 22, MiddleSize, 65, 130, 188, 64, 65, 1.0, FLinearColor::Yellow);
		DrawTexture(TextureAtlas, XOffset, YOffset + 22, EndSize, 65, 117, 188, 16, 65, 1.0, FLinearColor::Yellow);

		DrawText(NSLOCTEXT("UTDomScoreboard", "GoldTeam", "GOLD"), 1237, YOffset + CellHeight, UTHUDOwner->HugeFont, 1.0, 1.0, FLinearColor::White, ETextHorzPos::Right, ETextVertPos::Center);
		DrawText(FText::AsNumber(UTGameState->Teams[3]->Score), Size.X - Width + (Width * 0.1), YOffset + 48, UTHUDOwner->ScoreFont, false, FVector2D(0, 0), FLinearColor::Black, true, FLinearColor::Black, 0.75, 1.0, FLinearColor::White, ETextHorzPos::Left, ETextVertPos::Center);
	}
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

void UUTDomScoreboard::DrawMultiPlayerScores(float RenderDelta, float& YOffset)
{
	if (UTGameState == nullptr || UTGameState->Teams[2] == nullptr)
	{
		return;
	}

	int32 NumSpectators = 0;
	int32 XOffset = 0;
	AUTDomGameState* GS = Cast<AUTDomGameState>(UTGameState);
	if (GS == NULL || GS->NumTeams < 3) return;

	for (int8 Team = 2; Team < GS->NumTeams; Team++)
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
