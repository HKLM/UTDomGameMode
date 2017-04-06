// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTHUD_MultiTeam.h"
#include "UTDomGameState.h"
#include "ControlPoint.h"
#include "UTHUD_DOM.h"

AUTHUD_DOM::AUTHUD_DOM(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HudWidgetClasses.Empty(0);
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_Paperdoll.bpHW_Paperdoll_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_WeaponInfo.bpHW_WeaponInfo_C"));
	HudWidgetClasses.AddUnique(TEXT("/Script/MultiTeamGame.UTHUDWidget_MultiTeamClock"));
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
			DrawMinimapIcon(HUDAtlas, Pos, FVector2D(40.f, 40.f), MultiTeamIconUV[i], FVector2D(72.f, 72.f), GetLinearColorByTeamIndex(i), true);
			//TODO draw control point name
		}
	}
}
