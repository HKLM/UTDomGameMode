// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "Net/UnrealNetwork.h"
#include "CollisionQueryParams.h"
#include "UTDomGameMode.h"
#include "UTDomGameState.h"
#include "UTDomTeamInfo.h"
#include "UTDomStat.h"
#include "UTADomTypes.h"
#include "UTDomGameMessage.h"
#include "ControlPoint.h"
#include "UTDomControlPoint.h"

AUTDomControlPoint::AUTDomControlPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// StaticMesh assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint0Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomR.DomR'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint1Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomB.DomB'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint2Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomGN.DomGN'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint3Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomGold.DomGold'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint4Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomX.DomX'"));

	TeamMesh.Insert(ControlPoint0Mesh.Object, 0);
	TeamMesh.Insert(ControlPoint1Mesh.Object, 1);
	TeamMesh.Insert(ControlPoint2Mesh.Object, 2);
	TeamMesh.Insert(ControlPoint3Mesh.Object, 3);
	TeamMesh.Insert(ControlPoint4Mesh.Object, 4);

	// StaticMesh
	DomMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FName(TEXT("Mesh")));
	DomMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DomMesh->SetStaticMesh(ControlPoint4Mesh.Object);
	DomMesh->AlwaysLoadOnClient = true;
	DomMesh->AlwaysLoadOnServer = true;
	DomMesh->bCastDynamicShadow = true;
	DomMesh->bAffectDynamicIndirectLighting = true;
	DomMesh->bReceivesDecals = false;
	DomMesh->SetupAttachment(RootComponent);
	DomMesh->RelativeLocation.Z = 20;

	// Spinner
	MeshSpinner = ObjectInitializer.CreateDefaultSubobject<URotatingMovementComponent>(this, FName(TEXT("MeshSpinner")));
	MeshSpinner->UpdatedComponent = DomMesh;
	MeshSpinner->RotationRate.Yaw = 80.0f;

	static ConstructorHelpers::FObjectFinder<USoundBase> CaptureSound(TEXT("SoundCue'/UTDomGameMode/UTDomGameContent/Sounds/ControlSound_Cue.ControlSound_Cue'"));
	ControlPointCaptureSound = CaptureSound.Object;

	ScoreTime = 0.1f;
	MessageClass = UUTDomGameMessage::StaticClass();
	TeamNum = 255;
	SetReplicates(true);
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0;
	bScoreReady = true;
	bStopControlledTimer = false;
	bIsGameObjective = true;
}

void AUTDomControlPoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUTDomControlPoint, DomMesh);
}

void AUTDomControlPoint::Init(AControlPoint* Parent)
{
	bIsGameObjective = true;
}

void AUTDomControlPoint::BeginPlay()
{
	Super::BeginPlay();
	SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, 5.0f));
}

/**
 * @note this should not be called directly. Use AUTDomGameState->UpdateControlPointFX()
 * 		 for changes to be replicated to clients.
 */
void AUTDomControlPoint::UpdateTeamEffects_Implementation(uint8 TeamIndex)
{
	if (TeamMesh.IsValidIndex(TeamIndex))
	{
		DomMesh->SetStaticMesh(TeamMesh[TeamIndex]);
	}
	if (DomLightColor.IsValidIndex(TeamIndex))
	{
		DomLight->SetLightColor(DomLightColor[TeamIndex]);
	}

	Super::UpdateTeamEffects_Implementation(TeamIndex);
}

void AUTDomControlPoint::Disable_Implementation()
{
	DomMesh->SetVisibility(false);
	DomMesh->SetHiddenInGame(true);
	Super::Disable_Implementation();
}
