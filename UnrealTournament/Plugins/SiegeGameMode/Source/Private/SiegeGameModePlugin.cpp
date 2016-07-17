// Created by Brian 'Snake' Alexander, (c) 2016

#include "UnrealTournament.h"
#include "SiegeGameMode.h"
#include "Core.h"
#include "Engine.h"
#include "ModuleInterface.h"
#include "ModuleManager.h"

class FSiegeGameModePlugin : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FSiegeGameModePlugin, SiegeGameMode )

void FSiegeGameModePlugin::StartupModule()
{
}

void FSiegeGameModePlugin::ShutdownModule()
{
}
