// Created by Brian 'Snake' Alexander, 2017
#pragma once

#include "UnrealTournament.h"
#include "UTPathBuilderInterface.h"
#include "UTDomEndFocusActor.generated.h"

UCLASS(NotPlaceable)
class UTDOMGAMEMODE_API AUTDomEndFocusActor : public AActor, public IUTPathBuilderInterface
{
	GENERATED_BODY()
public:
	AUTDomEndFocusActor(const FObjectInitializer& OI)
		: Super(OI)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(FName(TEXT("SceneRoot")));
		SetReplicates(true);
		bReplicateMovement = true;
		bAlwaysRelevant = true;
		NetPriority = 3.0;
	}
};
