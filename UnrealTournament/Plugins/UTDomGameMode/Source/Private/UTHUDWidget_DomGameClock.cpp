#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTHUDWidget_TeamGameClock.h"
#include "UTHUDWidget_DomGameClock.h"

UUTHUDWidget_DomGameClock::UUTHUDWidget_DomGameClock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Position = FVector2D(0.0f, 0.0f);
	Size = FVector2D(430.0f, 83.0f);
	ScreenPosition = FVector2D(0.5f, 0.0f);
	Origin = FVector2D(0.5f, 0.0f);
	DesignedResolution = 1080.0;

	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDTex(TEXT("Texture2D'/Game/RestrictedAssets/UTDomGameContent/Textures/DomHUDAtlas01.DomHUDAtlas01'"));
	HUDDomAtlas = HUDTex.Object;

	BackgroundSlate.Atlas = HUDDomAtlas;
	BackgroundSlate.UVs.U = 192.0;
	BackgroundSlate.UVs.V = 229.0;
	BackgroundSlate.UVs.UL = 520.0;
	BackgroundSlate.UVs.VL = 83.0;
	BackgroundSlate.bIsSlateElement = true;
	BackgroundSlate.Position.X = -90.0;
	BackgroundSlate.Size.X = 600.0;
	BackgroundSlate.Size.Y = 86.5;
	BackgroundSlate.RenderColor = FLinearColor::Black;

	BackgroundBorder.Atlas = HUDDomAtlas;
	BackgroundBorder.UVs.U = 192.0;
	BackgroundBorder.UVs.V = 313.0;
	BackgroundBorder.UVs.UL = 430.0;
	BackgroundBorder.UVs.VL = 83.0;
	BackgroundBorder.TeamColorOverrides.Insert(FLinearColor(0.15f, 0.0f, 0.0f, 1.0f), 0);
	BackgroundBorder.TeamColorOverrides.Insert(FLinearColor(0.0f, 0.25f, 0.0f, 1.0f), 1);
	BackgroundBorder.TeamColorOverrides.Insert(FLinearColor(0.0f, 0.8f, 0.0f, 1.0f), 2);
	BackgroundBorder.TeamColorOverrides.Insert(FLinearColor(0.9f, 0.98f, 0.0f, 1.0f), 3);
	BackgroundBorder.bIsBorderElement = true;
	BackgroundBorder.Position.X = -61.0;
	BackgroundBorder.Size.X = 552.0;
	BackgroundBorder.RenderColor = FLinearColor::Black;

	GameStateBackground.Atlas = HUDDomAtlas;
	GameStateBackground.UVs.U = 76.0;
	GameStateBackground.UVs.V = 486.0;
	GameStateBackground.UVs.UL = 144.0;
	GameStateBackground.UVs.VL = 36.0;
	GameStateBackground.bIsSlateElement = true;
	GameStateBackground.bHidden = true;
	GameStateBackground.RenderPriority = -1.0;
	GameStateBackground.Position.X = 145.0;
	GameStateBackground.Position.Y = 20.0;

	GameStateText.HorzPosition = ETextHorzPos::Center;
	GameStateText.VertPosition = ETextVertPos::Center;
	GameStateText.Position.X = 225.0;
	GameStateText.Position.Y = 65.0;

	ClockBackground.Atlas = HUDDomAtlas;
	ClockBackground.UVs.U = 424.0;
	ClockBackground.UVs.V = 397.0;
	ClockBackground.UVs.UL = 82.0;
	ClockBackground.UVs.VL = 33.0;
	ClockBackground.TeamColorOverrides.Insert(FLinearColor(0.15f, 0.0f, 0.0f, 1.0f), 0);
	ClockBackground.TeamColorOverrides.Insert(FLinearColor(0.0f, 0.25f, 0.0f, 1.0f), 1);
	ClockBackground.TeamColorOverrides.Insert(FLinearColor(0.0f, 0.25f, 0.0f, 1.0f), 2);
	ClockBackground.TeamColorOverrides.Insert(FLinearColor(0.25f, 0.25f, 0.0f, 1.0f), 3);
	ClockBackground.bIsBorderElement = true;
	ClockBackground.bHidden = true;
	ClockBackground.RenderPriority = -1.0;
	ClockBackground.Position.X = 161.0;
	ClockBackground.Size.X = 111.0;
	ClockBackground.Size.Y = 62.0;
	ClockBackground.RenderColor = FLinearColor(0.082353, 0.082353, 0.082353, 1.0);

	ClockText.HorzPosition = ETextHorzPos::Center;
	ClockText.VertPosition = ETextVertPos::Center;
	ClockText.Position.X = 224.0;
	ClockText.Position.Y = 32.0;

	RedTeamBanner.Atlas = HUDDomAtlas;
	RedTeamBanner.UVs.U = 11.0;
	RedTeamBanner.UVs.V = 485.0;
	RedTeamBanner.UVs.UL = 19.0;
	RedTeamBanner.UVs.VL = 70.0;
	RedTeamBanner.RenderPriority = 1.0;
	RedTeamBanner.bHidden = true;
	RedTeamBanner.Position.X = 3.0;
	RedTeamBanner.Size.Y = 75.0;
	RedTeamBanner.RenderColor = FLinearColor::White;

	BlueTeamBanner.Atlas = HUDDomAtlas;
	BlueTeamBanner.UVs.U = 35.0;
	BlueTeamBanner.UVs.V = 485.0;
	BlueTeamBanner.UVs.UL = 19.0;
	BlueTeamBanner.UVs.VL = 70.0;
	BlueTeamBanner.RenderPriority = 1.0;
	BlueTeamBanner.bHidden = true;
	BlueTeamBanner.Position.X = 408.0;
	BlueTeamBanner.Size.Y = 75.0;
	BlueTeamBanner.RenderColor = FLinearColor::White;

	RedScoreText.HorzPosition = ETextHorzPos::Center;
	RedScoreText.VertPosition = ETextVertPos::Center;
	RedScoreText.Position.X = 80.0;
	RedScoreText.Position.Y = 30.0;
	RedScoreText.RenderColor = FLinearColor(0.35f, 0.02f, 0.02f, 1.0f);

	BlueScoreText.HorzPosition = ETextHorzPos::Center;
	BlueScoreText.VertPosition = ETextVertPos::Center;
	BlueScoreText.Position.X = 340.0;
	BlueScoreText.Position.Y = 30.0;
	BlueScoreText.RenderColor = FLinearColor(0.1f, 0.1f, 0.6f, 1.0f);

	AltClockScale = 0.5;
}

void UUTHUDWidget_DomGameClock::InitializeWidget(AUTHUD* Hud)
{
	Super::InitializeWidget(Hud);
}

void UUTHUDWidget_DomGameClock::Draw_Implementation(float DeltaTime)
{
	if (UTHUDOwner)
	{
		if (GameStateText.Font == NULL || UTHUDOwner->HUDAtlas != HUDDomAtlas)
		{
			GameStateText.Font = UTHUDOwner->TinyFont;
			ClockText.Font = UTHUDOwner->NumberFont;
			RedScoreText.Font = UTHUDOwner->HugeFont;
			BlueScoreText.Font = UTHUDOwner->HugeFont;
			UTHUDOwner->HUDAtlas = HUDDomAtlas;
		}
	}
	Super::Draw_Implementation(DeltaTime);
}
