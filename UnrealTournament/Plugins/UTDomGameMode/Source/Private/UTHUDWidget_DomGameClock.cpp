// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTHUDWidget.h"
#include "UTHUDWidget_DomGameClock.h"

UUTHUDWidget_DomGameClock::UUTHUDWidget_DomGameClock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Position = FVector2D(0.0f, 0.0f);
	Size = FVector2D(760.0f, 83.0f);
	ScreenPosition = FVector2D(0.5f, 0.0f);
	Origin = FVector2D(0.5f, 0.0f);
	DesignedResolution = 1080.0;

	static ConstructorHelpers::FObjectFinder<UTexture2D> DHUDTex(TEXT("Texture'/UTDomGameMode/UTDomGameContent/Textures/DOM_HUDAtlas01.DOM_HUDAtlas01'"));
	DomAtlas = DHUDTex.Object;

	static ConstructorHelpers::FObjectFinder<UFont> HFont(TEXT("Font'/Game/RestrictedAssets/UI/Fonts/fntScoreboard_Huge.fntScoreboard_Huge'"));
	static ConstructorHelpers::FObjectFinder<UFont> CFont(TEXT("Font'/Game/RestrictedAssets/UI/Fonts/fntScoreboard_Clock.fntScoreboard_Clock'"));
	static ConstructorHelpers::FObjectFinder<UFont> TinyFont(TEXT("Font'/Game/RestrictedAssets/UI/Fonts/fntScoreboard_Tiny.fntScoreboard_Tiny'"));

	static ConstructorHelpers::FObjectFinder<UTexture2D> Tex0(TEXT("Texture2D'/UTDomGameMode/UTDomGameContent/Textures/RedTeamSymbol.RedTeamSymbol'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Tex1(TEXT("Texture2D'/UTDomGameMode/UTDomGameContent/Textures/BlueTeamSymbol.BlueTeamSymbol'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Tex2(TEXT("Texture2D'/UTDomGameMode/UTDomGameContent/Textures/GreenTeamSymbol.GreenTeamSymbol'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Tex3(TEXT("Texture2D'/UTDomGameMode/UTDomGameContent/Textures/GoldTeamSymbol.GoldTeamSymbol'"));
	DomTeamIcon.Insert(Tex0.Object, 0);
	DomTeamIcon.Insert(Tex1.Object, 1);
	DomTeamIcon.Insert(Tex2.Object, 2);
	DomTeamIcon.Insert(Tex3.Object, 3);

	BackgroundSlate.Atlas = DomAtlas;
	BackgroundSlate.UVs.U = 192.0;
	BackgroundSlate.UVs.V = 229.0;
	BackgroundSlate.UVs.UL = 520.0;
	BackgroundSlate.UVs.VL = 83.0;
	BackgroundSlate.bIsSlateElement = true;
	BackgroundSlate.bHidden = true;
	BackgroundSlate.Position.X = -90.0;
	BackgroundSlate.Size.X = 900.0;
	BackgroundSlate.Size.Y = 75.0;
	BackgroundSlate.RenderColor = FLinearColor::Black;

	BackgroundBorder.Atlas = DomAtlas;
	BackgroundBorder.UVs.U = 192.0;
	BackgroundBorder.UVs.V = 313.0;
	BackgroundBorder.UVs.UL = 430.0;
	BackgroundBorder.UVs.VL = 83.0;
	BackgroundBorder.TeamColorOverrides.Insert(FLinearColor(0.15f, 0.0f, 0.0f, 1.0f), 0);
	BackgroundBorder.TeamColorOverrides.Insert(FLinearColor(0.0f, 0.0f, 0.15f, 1.0f), 1);
	BackgroundBorder.TeamColorOverrides.Insert(FLinearColor(0.0f, 0.8f, 0.0f, 1.0f), 2);
	BackgroundBorder.TeamColorOverrides.Insert(FLinearColor(0.9f, 0.98f, 0.0f, 1.0f), 3);
	BackgroundBorder.bIsBorderElement = true;
	BackgroundBorder.Position.X = -54.0;
	BackgroundBorder.Size.X = 755.0;
	BackgroundBorder.Size.Y = 75.0;
	BackgroundBorder.RenderColor = FLinearColor::Black;
	BackgroundBorder.RenderOpacity = 0.75f;
	BackgroundBorder.bHidden = true;

	GameStateText.Font = TinyFont.Object;
	GameStateText.HorzPosition = ETextHorzPos::Center;
	GameStateText.VertPosition = ETextVertPos::Center;
	GameStateText.Position.X = 225.0;
	GameStateText.Position.Y = 73.0;

	ClockText.Font = CFont.Object;
	ClockText.HorzPosition = ETextHorzPos::Center;
	ClockText.VertPosition = ETextVertPos::Center;
	ClockText.Position.X = 5.0;
	ClockText.Position.Y = 44.0;
	ClockText.Size.X = 80.0;
	ClockText.Size.Y = 12.0;
	ClockText.RenderPriority = 1.0;
	ClockText.RenderScale = 1.0;

	RedTeamLogo.Atlas = DomTeamIcon[0];
	RedTeamLogo.UVs.U = 0.0;
	RedTeamLogo.UVs.V = 0.0;
	RedTeamLogo.UVs.UL = 256.0;
	RedTeamLogo.UVs.VL = 256.0;
	RedTeamLogo.RenderPriority = 1.0;
	RedTeamLogo.Position.X = 100.0;
	RedTeamLogo.Position.Y = 30.0;
	RedTeamLogo.Size.X = 38.0;
	RedTeamLogo.Size.Y = 38.0;
	RedTeamLogo.RenderColor = FLinearColor::White;

	BlueTeamLogo.Atlas = DomTeamIcon[1];
	BlueTeamLogo.UVs.U = 0.0;
	BlueTeamLogo.UVs.V = 0.0;
	BlueTeamLogo.UVs.UL = 256.0;
	BlueTeamLogo.UVs.VL = 256.0;
	BlueTeamLogo.RenderPriority = 1.0;
	BlueTeamLogo.Position.X = 240.0;
	BlueTeamLogo.Position.Y = 30.0;
	BlueTeamLogo.Size.X = 38.0;
	BlueTeamLogo.Size.Y = 38.0;
	BlueTeamLogo.RenderColor = FLinearColor::White;

	GreenTeamLogo.Atlas = DomTeamIcon[2];
	GreenTeamLogo.UVs.U = 0.0;
	GreenTeamLogo.UVs.V = 0.0;
	GreenTeamLogo.UVs.UL = 256.0;
	GreenTeamLogo.UVs.VL = 256.0;
	GreenTeamLogo.RenderPriority = 1.0;
	GreenTeamLogo.Position.X = 380;
	GreenTeamLogo.Position.Y = 30;
	GreenTeamLogo.Size.X = 38.0;
	GreenTeamLogo.Size.Y = 38.0;
	GreenTeamLogo.RenderColor = FLinearColor::White;
	GreenTeamLogo.bHidden = true;

	GoldTeamLogo.Atlas = DomTeamIcon[3];
	GoldTeamLogo.UVs.U = 0.0;
	GoldTeamLogo.UVs.V = 0.0;
	GoldTeamLogo.UVs.UL = 256.0;
	GoldTeamLogo.UVs.VL = 256.0;
	GoldTeamLogo.RenderPriority = 1.0;
	GoldTeamLogo.Position.X = 520;
	GoldTeamLogo.Position.Y = 30;
	GoldTeamLogo.Size.X = 38.0;
	GoldTeamLogo.Size.Y = 38.0;
	GoldTeamLogo.RenderColor = FLinearColor::White;
	GoldTeamLogo.bHidden = true;

	RedScoreText.Font = HFont.Object;
	RedScoreText.HorzPosition = ETextHorzPos::Left;
	RedScoreText.VertPosition = ETextVertPos::Center;
	RedScoreText.RenderPriority = 1.0;
	RedScoreText.Position.X = 135.0;
	RedScoreText.Position.Y = 30.0;
	RedScoreText.Size.X = 100.0;
	RedScoreText.Size.Y = 30.0;
	RedScoreText.RenderColor = FLinearColor(0.35f, 0.02f, 0.02f, 1.0f);

	BlueScoreText.Font = HFont.Object;
	BlueScoreText.HorzPosition = ETextHorzPos::Left;
	BlueScoreText.VertPosition = ETextVertPos::Center;
	BlueScoreText.RenderPriority = 1.0;
	BlueScoreText.Position.X = 275.0;
	BlueScoreText.Position.Y = 30.0;
	BlueScoreText.Size.X = 100.0;
	BlueScoreText.Size.Y = 30.0;
	BlueScoreText.RenderColor = FLinearColor(0.1f, 0.1f, 0.6f, 1.0f);

	GreenScoreText.Font = HFont.Object;
	GreenScoreText.HorzPosition = ETextHorzPos::Left;
	GreenScoreText.VertPosition = ETextVertPos::Center;
	GreenScoreText.RenderPriority = 1.0;
	GreenScoreText.Position.X = 415.0;
	GreenScoreText.Position.Y = 30.0;
	GreenScoreText.Size.X = 100.0;
	GreenScoreText.Size.Y = 30.0;
	GreenScoreText.RenderColor = FLinearColor(0.01f, 0.25f, 0.01f, 1.0f);
	GreenScoreText.bHidden = true;

	GoldScoreText.Font = HFont.Object;
	GoldScoreText.HorzPosition = ETextHorzPos::Left;
	GoldScoreText.VertPosition = ETextVertPos::Center;
	GoldScoreText.RenderPriority = 1.0;
	GoldScoreText.Position.X = 555.0;
	GoldScoreText.Position.Y = 30.0;
	GoldScoreText.Size.X = 100.0;
	GoldScoreText.Size.Y = 24.0;
	GoldScoreText.RenderColor = FLinearColor(0.6f, 0.5f, 0.0f, 1.0f);
	GoldScoreText.bHidden = true;

	RoleText.Text = FText::FromString("You are on");
	RoleText.Font = TinyFont.Object;
	RoleText.HorzPosition = ETextHorzPos::Left;
	RoleText.VertPosition = ETextVertPos::Top;
	RoleText.bDrawShadow = true;
	RoleText.ShadowDirection.X = 1.0f;
	RoleText.ShadowDirection.Y = 1.0f;
	RoleText.ShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
	RoleText.Position.X = -35.0f;
	RoleText.Position.Y = 10.0f;
	RoleText.Size.X = 50.0;
	RoleText.Size.Y = 20.0f;
	RoleText.RenderColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//TeamNameText.Text = "RED";
	TeamNameText.Font = TinyFont.Object;
	TeamNameText.HorzPosition = ETextHorzPos::Left;
	TeamNameText.VertPosition = ETextVertPos::Top;
	TeamNameText.ShadowDirection.X = 1.0f;
	TeamNameText.ShadowDirection.Y = 1.0f;
	TeamNameText.ShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
	TeamNameText.bDrawOutline = true;
	TeamNameText.Position.X = 55.0;
	TeamNameText.Position.Y = 10.0f;
	TeamNameText.Size.X = 40.0;
	TeamNameText.Size.Y = 20.0f;

	AltClockScale = 0.5;
}

void UUTHUDWidget_DomGameClock::InitializeWidget(AUTHUD* Hud)
{
	Super::InitializeWidget(Hud);

	RedScoreText.GetTextDelegate.BindUObject(this, &UUTHUDWidget_DomGameClock::GetRedScoreText);
	BlueScoreText.GetTextDelegate.BindUObject(this, &UUTHUDWidget_DomGameClock::GetBlueScoreText);

	GreenScoreText.GetTextDelegate.BindUObject(this, &UUTHUDWidget_DomGameClock::GetGreenScoreText);
	GoldScoreText.GetTextDelegate.BindUObject(this, &UUTHUDWidget_DomGameClock::GetGoldScoreText);

	ClockText.GetTextDelegate.BindUObject(this, &UUTHUDWidget_DomGameClock::GetClockText);
}

void UUTHUDWidget_DomGameClock::Draw_Implementation(float DeltaTime)
{
	FText StatusText = FText::GetEmpty();
	if (UTGameState != nullptr)
	{
		StatusText = UTGameState->GetGameStatusText(false);
	}

	if (!StatusText.IsEmpty())
	{
		GameStateText.bHidden = false;
		GameStateText.Text = StatusText;
		GameStateText.RenderColor = UTGameState->GetGameStatusColor();
	}
	else
	{
		GameStateText.bHidden = true;
		GameStateText.Text = StatusText;
	}

	AUTCharacter* UTC = Cast<AUTCharacter>(UTHUDOwner->UTPlayerOwner->GetViewTarget());
	AUTPlayerState* PS = UTC ? Cast<AUTPlayerState>(UTC->PlayerState) : NULL;
	if (UTGameState && UTGameState->bTeamGame && PS && PS->Team)
	{
		TeamNameText.Text = PS->Team->TeamName;
		TeamNameText.RenderColor = PS->Team->TeamColor;
	}

	TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
	if (GenericApplication.IsValid() && !GenericApplication->IsUsingHighPrecisionMouseMode() && UTHUDOwner->PlayerOwner && UTHUDOwner->PlayerOwner->PlayerState && !UTHUDOwner->PlayerOwner->PlayerState->bOnlySpectator)
	{
		Canvas->SetDrawColor(FColor::Red);
		Canvas->DrawTile(Canvas->DefaultTexture, Canvas->ClipX - 5, Canvas->ClipY - 5, 5, 5, 0.0f, 0.0f, 1.0f, 1.0f);
	}

	if (UTGameState)
	{
		if (UTGameState->Teams.Num() == 4)
		{
			GreenTeamLogo.bHidden = false;
			GreenScoreText.bHidden = false;
			GoldTeamLogo.bHidden = false;
			GoldScoreText.bHidden = false;
		}
		else if (UTGameState->Teams.Num() == 3)
		{
			GreenTeamLogo.bHidden = false;
			GreenScoreText.bHidden = false;
			GoldTeamLogo.bHidden = true;
			GoldScoreText.bHidden = true;
		}
	}

	Super::Draw_Implementation(DeltaTime);
}

FText UUTHUDWidget_DomGameClock::GetRedScoreText_Implementation()
{
	if (UTGameState && UTGameState->bTeamGame && UTGameState->Teams.Num() > 0 && UTGameState->Teams[0])
	{
		return FText::AsNumber(UTGameState->Teams[0]->Score);
	}
	return FText::AsNumber(0);
}

FText UUTHUDWidget_DomGameClock::GetBlueScoreText_Implementation()
{
	if (UTGameState && UTGameState->bTeamGame && UTGameState->Teams.Num() > 1 && UTGameState->Teams[1])
	{
		return FText::AsNumber(UTGameState->Teams[1]->Score);
	}
	return FText::AsNumber(0);
}

FText UUTHUDWidget_DomGameClock::GetClockText_Implementation()
{
	float RemainingTime = UTGameState ? UTGameState->GetClockTime() : 0.f;
	FText ClockString = UTHUDOwner->ConvertTime(FText::GetEmpty(), FText::GetEmpty(), RemainingTime, false);
	ClockText.TextScale = (RemainingTime >= 3600) ? AltClockScale : GetClass()->GetDefaultObject<UUTHUDWidget_DomGameClock>()->ClockText.TextScale;
	return ClockString;
}

FText UUTHUDWidget_DomGameClock::GetGreenScoreText_Implementation()
{
	if (UTGameState && UTGameState->Teams.Num() > 2 && UTGameState->Teams.IsValidIndex(2) && UTGameState->Teams[2])
	{
		return FText::AsNumber(UTGameState->Teams[2]->Score);
	}
	return FText::AsNumber(0);
}

FText UUTHUDWidget_DomGameClock::GetGoldScoreText_Implementation()
{
	if (UTGameState && UTGameState->Teams.Num() > 3 && UTGameState->Teams.IsValidIndex(3) && UTGameState->Teams[3])
	{
		return FText::AsNumber(UTGameState->Teams[3]->Score);
	}
	return FText::AsNumber(0);
}
