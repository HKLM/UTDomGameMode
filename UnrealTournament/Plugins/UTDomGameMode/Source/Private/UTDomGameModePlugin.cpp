// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameMode.h"
#include "Core.h"
#include "Engine.h"
#include "ModuleInterface.h"
#include "ModuleManager.h"

class UTDOMGAMEMODE_API FUTDomGameModePlugin : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FUTDomGameModePlugin, UTDomGameMode )

void FUTDomGameModePlugin::StartupModule()
{
}

void FUTDomGameModePlugin::ShutdownModule()
{
}
