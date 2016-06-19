// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTHUD_DOM.h"

AUTHUD_DOM::AUTHUD_DOM(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_Paperdoll.bpHW_Paperdoll_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_WeaponInfo.bpHW_WeaponInfo_C"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UTDomGameMode.UTHUDWidget_DomGameClock"));
	//HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_TeamGameClock.bpHW_TeamGameClock_C"));
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

