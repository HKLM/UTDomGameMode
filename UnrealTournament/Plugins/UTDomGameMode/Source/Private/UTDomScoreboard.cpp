// Created by Brian 'Snake' Alexander, 2015

#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTDomScoreboard.h"

UUTDomScoreboard::UUTDomScoreboard(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
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
			FText Score = FText::Format(NSLOCTEXT("UTDomScoreboard", "GoalScoreFormat", "Score most points in {0} minutes"), FText::AsNumber(UTGameState->TimeLimit));
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
