// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameMode.h"
#include "ControlPoint.h"
#include "Net/UnrealNetwork.h"
#include "UTGameObjective.h"
#include "DominationObjective.h"

ADominationObjective::ADominationObjective(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint4Mesh(TEXT("StaticMesh'/Game/RestrictedAssets/UTDomGameContent/Meshes/DomX.DomX'"));
	EditorMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FName(TEXT("DomEditorMesh")));
	EditorMesh->AttachParent = RootComponent;
	EditorMesh->SetStaticMesh(ControlPoint4Mesh.Object);
	EditorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EditorMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
	EditorMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
	EditorMesh->SetHiddenInGame(true);

	DomLight = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, FName(TEXT("DomEditorLight")));
	DomLight->AttachParent = RootComponent;
	DomLight->SetAttenuationRadius(900.0f);
	DomLight->bUseInverseSquaredFalloff = false;
	DomLight->SetAffectDynamicIndirectLighting(true);
	DomLight->SetIntensity(10.0f);
	DomLight->RelativeLocation = FVector(0.0f, 0.0f, 90.0f);
	DomLight->SetLightColor(FLinearColor::Gray);
	DomLight->bHiddenInGame = true;

	static ConstructorHelpers::FObjectFinder<USoundBase> CaptureSound(TEXT("SoundCue'/Game/RestrictedAssets/UTDomGameContent/Sounds/ControlSound_Cue.ControlSound_Cue'"));
	ControlPointCaptureSound = CaptureSound.Object;

	SetReplicates(true);
	bAlwaysRelevant = true;
	NetPriority = 1.0;
}

void ADominationObjective::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ADominationObjective, MyDomObjectiveType, COND_None);
	DOREPLIFETIME(ADominationObjective, MyControlPoint);
}

void ADominationObjective::InitializeObjective()
{
	// determine the type of ControlPoint needed to spawn by checking the game mode
	if (MyDomObjectiveType == NULL)
	{
		AUTDomGameMode* Game = GetWorld()->GetAuthGameMode<AUTDomGameMode>();
		if (Game != NULL && Game->DominationObjectiveType != NULL)
		{
			MyDomObjectiveType = Game->DominationObjectiveType;
		}
	}
	CarriedObjectClass = MyDomObjectiveType;

	Super::InitializeObjective();
}

void ADominationObjective::CreateCarriedObject()
{
	if (DomLight)
	{
		DomLight->SetAttenuationRadius(0.0f);
		DomLight->SetIntensity(0.0f);
		DomLight->SetVisibility(false);
		DomLight->SetActive(false);
	}
	if (EditorMesh)
	{
		EditorMesh->SetHiddenInGame(true);
		EditorMesh->SetVisibility(false);
	}
	// determine the type of ControlPoint needed to spawn by checking the game mode
	if (MyDomObjectiveType == NULL)
	{
		AUTDomGameMode* Game = GetWorld()->GetAuthGameMode<AUTDomGameMode>();
		if (Game != NULL)
		{
			MyDomObjectiveType = Game->DominationObjectiveType;
		}
	}

	Super::CreateCarriedObject();
	if (CarriedObject != NULL)
	{
		if (MyDomObjectiveType != NULL)
		{
			MyControlPoint = Cast<AControlPoint>(CarriedObject);
			if (MyControlPoint != NULL)
			{
				MyControlPoint->PointName = PointName;
				if (ControlPointCaptureSound)
				{
					MyControlPoint->ControlPointCaptureSound = ControlPointCaptureSound;
				}
			}
		}
	}
}

int32 ADominationObjective::GetControllingTeamNum()
{
	return (MyControlPoint != NULL) ? MyControlPoint->ControllingTeamNum : 255;
}

AUTPlayerState* ADominationObjective::GetControlPointHolder()
{
	return (MyControlPoint != NULL) ? MyControlPoint->GetControlPointHolder() : NULL;
}

AUTPlayerState* ADominationObjective::GetCarriedObjectHolder()
{
	return GetControlPointHolder();
}

AControlPoint* ADominationObjective::GetControlPoint()
{
	return (MyControlPoint != NULL) ? MyControlPoint : NULL;
}

FString ADominationObjective::GetPointName()
{
	return (PointName.IsEmpty()) ? TEXT("ControlPoint") : PointName;
}

FName ADominationObjective::GetCarriedObjectState() const
{
	return MyControlPoint->GetTeamEventName(MyControlPoint->ControllingTeamNum);
}
