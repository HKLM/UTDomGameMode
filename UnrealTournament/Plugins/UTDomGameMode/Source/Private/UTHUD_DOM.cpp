// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTHUD_DOM.h"

AUTHUD_DOM::AUTHUD_DOM(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_Paperdoll.bpHW_Paperdoll_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_WeaponInfo.bpHW_WeaponInfo_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_TeamGameClock.bpHW_TeamGameClock_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_WeaponBar.bpHW_WeaponBar_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_Powerups.bpHW_Powerups_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_FloatingScore.bpHW_FloatingScore_C"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidget_WeaponCrosshair"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UTDomGameMode.UTHUDWidget_DOMStatus"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidgetMessage_DeathMessages"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidgetMessage_PickupMessage"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidgetMessage_ConsoleMessages"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidgetMessage_GameMessages"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidget_Spectator"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UTDomGameMode.UTDomScoreboard"));
}

FLinearColor AUTHUD_DOM::GetBaseHUDColor()
{
	FLinearColor TeamColor = Super::GetBaseHUDColor();

	AUTPlayerState* PS = Cast<AUTPlayerState>(UTPlayerOwner->PlayerState);
	if (PS != NULL && PS->Team != NULL)
	{
		TeamColor = PS->Team->TeamColor;
	}
	return TeamColor;
}
