// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameMode.h"
#include "Core.h"
#include "Engine.h"
#include "ModuleManager.h"
#include "ModuleInterface.h"

class FUTDomGameModePlugin: public IModuleInterface
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
