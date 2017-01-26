// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "ControlPoint.h"
#include "UTHUD_DOM.h"

AUTHUD_DOM::AUTHUD_DOM(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> DHUDTex(TEXT("Texture'/UTDomGameMode/UTDomGameContent/Textures/DOM_HUDAtlas01.DOM_HUDAtlas01'"));
	HUDDomAtlas = DHUDTex.Object;

	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_Paperdoll.bpHW_Paperdoll_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_WeaponInfo.bpHW_WeaponInfo_C"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UTDomGameMode.UTHUDWidget_DomGameClock"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_WeaponBar.bpHW_WeaponBar_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_Powerups.bpHW_Powerups_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_FloatingScore.bpHW_FloatingScore_C"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidget_WeaponCrosshair"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidgetMessage_ConsoleMessages"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UTDomGameMode.UTHUDWidget_DOMStatus"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidgetAnnouncements"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpWH_KillIconMessages.bpWH_KillIconMessages_C"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidget_Spectator"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UTDomGameMode.UTDomScoreboard"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_QuickStats.bpHW_QuickStats_C"));

	DOMTeamIconUV[0] = FVector2D(257.f, 940.f);
	DOMTeamIconUV[1] = FVector2D(333.f, 940.f);
	DOMTeamIconUV[2] = FVector2D(257.f, 853.f);
	DOMTeamIconUV[3] = FVector2D(333.f, 853.f);
	DOMTeamIconUV[4] = FVector2D(256.f, 770.f);
}

void AUTHUD_DOM::BeginPlay()
{
	HUDAtlas = HUDDomAtlas;
	Super::BeginPlay();
}

FLinearColor AUTHUD_DOM::GetBaseHUDColor()
{
	FLinearColor TeamColor = Super::GetBaseHUDColor();
	APawn* HUDPawn = Cast<APawn>(UTPlayerOwner->GetViewTarget());
	if (HUDPawn)
	{
		AUTPlayerState* PS = Cast<AUTPlayerState>(HUDPawn->PlayerState);
		if (PS != NULL && PS->Team != NULL)
		{
			TeamColor = PS->Team->TeamColor;
		}
	}
	return TeamColor;
}

FLinearColor AUTHUD_DOM::GetWidgetTeamColor()
{
	// Add code to cache and return the team color if it's a team game
	AUTGameState* GS = GetWorld()->GetGameState<AUTGameState>();
	if (GS == NULL || (GS->bTeamGame && UTPlayerOwner && UTPlayerOwner->GetViewTarget()))
	{
		APawn* HUDPawn = Cast<APawn>(UTPlayerOwner->GetViewTarget());
		AUTPlayerState* PS = HUDPawn ? Cast<AUTPlayerState>(HUDPawn->PlayerState) : NULL;
		if (PS != NULL)
		{
			switch (PS->GetTeamNum())
			{
			case 0: return FLinearColor(0.15, 0.0, 0.0, 1.0); break;
			case 1: return FLinearColor(0.025, 0.025, 0.1, 1.0); break;
			case 2: return FLinearColor(0.0, 0.25, 0.0, 1.0); break;
			case 3: return FLinearColor(0.25, 0.25, 0.0, 1.0); break;
			default: break;
			}
		}
	}

	return FLinearColor::Black;
}

FColor AUTHUD_DOM::GetColorByTeamIndex(int32 TeamIndex)
{
	switch (TeamIndex)
	{
		case 0: return FColor(255, 0, 0, 255); break;
		case 1: return FColor(0, 0, 255, 255); break;
		case 2: return FColor(0, 255, 0, 255); break;
		case 3: return FColor(255, 255, 0, 255); break;
		case 5: return FColor(0, 0, 0, 255); break;
		default: return FColor(155, 155, 155, 255); break;
	}
}

FLinearColor AUTHUD_DOM::GetLinearColorByTeamIndex(int32 TeamIndex)
{
	switch (TeamIndex)
	{
		case 0: return FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); break;
		case 1: return FLinearColor(0.025f, 0.025f, 1.0f, 1.0f); break;
		case 2: return FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); break;
		case 3: return FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); break;
		case 4: return FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); break;
		default: return FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); break;
	}
}

void AUTHUD_DOM::DrawMinimapSpectatorIcons()
{
	Super::DrawMinimapSpectatorIcons();

	AUTDomGameState* GS = Cast<AUTDomGameState>(GetWorld()->GetGameState());
	if (GS == NULL) return;

	TArray<AControlPoint*> ControlPts = GS->AUTDomGameState::GetControlPoints();	
	if (ControlPts.Num() > 0)
	{
		for (int32 CPIndex = 0; CPIndex < ControlPts.Num(); CPIndex++)
		{
			uint8 i = ControlPts[CPIndex]->GetTeamNum();
			FVector2D Pos = WorldToMapToScreen(ControlPts[CPIndex]->GetActorLocation());
			DrawMinimapIcon(HUDAtlas, Pos, FVector2D(40.f, 40.f), DOMTeamIconUV[i], FVector2D(72.f, 72.f), GetLinearColorByTeamIndex(i), true);
			//TODO draw control point name
		}
	}
}
