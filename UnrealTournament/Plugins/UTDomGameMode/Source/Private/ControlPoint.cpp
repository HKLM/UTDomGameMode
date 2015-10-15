// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTDomGameMode.h"
#include "UTDomGameMessage.h"
#include "UTDomTeamInfo.h"
#include "Net/UnrealNetwork.h"
#include "CollisionQueryParams.h"
#include "ControlPoint.h"

FCollisionResponseParams WorldResponseParams = []()
{
	FCollisionResponseParams Result(ECR_Ignore);
	Result.CollisionResponse.WorldStatic = ECR_Block;
	Result.CollisionResponse.WorldDynamic = ECR_Block;
	return Result;
}();

AControlPoint::AControlPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SceneRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent, USceneComponent>(this, TEXT("DummyRoot"), false);
	RootComponent = SceneRoot;

	DomCollision = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("Collision"));
	DomCollision->InitCapsuleSize(90.0f, 140.0f);
	DomCollision->SetCollisionProfileName(FName(TEXT("Pickup")));
	DomCollision->AttachParent = RootComponent;
	DomCollision->RelativeLocation.Z = 140.0f;
	DomCollision->OnComponentBeginOverlap.AddDynamic(this, &AControlPoint::OnOverlapBegin);
	DomCollision->OnComponentEndOverlap.AddDynamic(this, &AControlPoint::OnOverlapEnd_Implementation);

	// Load StaticMesh assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint0Mesh(TEXT("StaticMesh'/Game/RestrictedAssets/UTDomGameContent/Meshes/DomR.DomR'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint1Mesh(TEXT("StaticMesh'/Game/RestrictedAssets/UTDomGameContent/Meshes/DomB.DomB'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint2Mesh(TEXT("StaticMesh'/Game/RestrictedAssets/UTDomGameContent/Meshes/DomGN.DomGN'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint3Mesh(TEXT("StaticMesh'/Game/RestrictedAssets/UTDomGameContent/Meshes/DomGold.DomGold'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint4Mesh(TEXT("StaticMesh'/Game/RestrictedAssets/UTDomGameContent/Meshes/DomX.DomX'"));

	TeamMesh.Insert(ControlPoint0Mesh.Object, 0);
	TeamMesh.Insert(ControlPoint1Mesh.Object, 1);
	TeamMesh.Insert(ControlPoint2Mesh.Object, 2);
	TeamMesh.Insert(ControlPoint3Mesh.Object, 3);
	TeamMesh.Insert(ControlPoint4Mesh.Object, 4);

	DomLightColor.Insert(FLinearColor::Red, 0);
	DomLightColor.Insert(FLinearColor::Blue, 1);
	DomLightColor.Insert(FLinearColor::Green, 2);
	DomLightColor.Insert(FLinearColor::Yellow, 3);
	DomLightColor.Insert(FLinearColor::Gray, 4);

	DomMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FName(TEXT("Mesh")));
	DomMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DomMesh->SetStaticMesh(ControlPoint4Mesh.Object);
	DomMesh->AlwaysLoadOnClient = true;
	DomMesh->AlwaysLoadOnServer = true;
	DomMesh->bCastDynamicShadow = true;
	DomMesh->bAffectDynamicIndirectLighting = true;
	DomMesh->bReceivesDecals = false;
	DomMesh->AttachParent = RootComponent;
	DomMesh->RelativeLocation.Z = 10.0f;

	// Spinner
	MeshSpinner = ObjectInitializer.CreateDefaultSubobject<URotatingMovementComponent>(this, FName(TEXT("MeshSpinner")));
	MeshSpinner->UpdatedComponent = DomMesh;
	MeshSpinner->RotationRate.Yaw = 80.0f;

	// Light
	DomLight = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, FName(TEXT("Light")));
	DomLight->AttachParent = RootComponent;
	DomLight->RelativeLocation.Z = 90.0f;
	DomLight->SetAttenuationRadius(900.0f);
	DomLight->bUseInverseSquaredFalloff = false;
	DomLight->SetAffectDynamicIndirectLighting(true);
	DomLight->SetIntensity(10.0f);
	DomLight->SetLightColor(DomLightColor[4]);

	static ConstructorHelpers::FObjectFinder<USoundBase> CaptureSound(TEXT("SoundCue'/Game/RestrictedAssets/UTDomGameContent/Sounds/ControlSound_Cue.ControlSound_Cue'"));
	ControlPointCaptureSound = CaptureSound.Object;

	ScoreTime = 2.0f;
	MessageClass = UUTDomGameMessage::StaticClass();
	ControllingTeamNum = 255;
	SetReplicates(true);
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0;
}

void AControlPoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AControlPoint, DomMesh);
	DOREPLIFETIME(AControlPoint, ScoreTime);
	DOREPLIFETIME(AControlPoint, ControllingPawn);
	DOREPLIFETIME(AControlPoint, ControllingTeam);
	DOREPLIFETIME(AControlPoint, ControllingTeamNum);
	DOREPLIFETIME_CONDITION(AControlPoint, PointName, COND_InitialOnly);
}

void AControlPoint::BeginPlay()
{
	Super::BeginPlay();
	SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, 25.0f));
}

FString AControlPoint::GetPointName()
{
	return (PointName.IsEmpty()) ? TEXT("ControlPoint") : PointName;
}

AUTPlayerState* AControlPoint::GetControlPointHolder()
{
	return ControllingPawn;
}

int32 AControlPoint::GetControllingTeamNum()
{
	if (ControllingTeamNum == 255)
	{
		return 4;
	}
	else
	{
		return ControllingTeamNum;
	}
}

void AControlPoint::OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AUTCharacter* Character = Cast<AUTCharacter>(OtherActor);
	if (Character != NULL && !GetWorld()->LineTraceTestByChannel(OtherActor->GetActorLocation(), GetActorLocation(), ECC_Pawn, FCollisionQueryParams(), WorldResponseParams))
	{
		APawn* P = Cast<APawn>(OtherActor);
		if (P != NULL)
		{
			ProcessTouch(Character);
		}
	}
}

void AControlPoint::ProcessTouch_Implementation(APawn* TouchedBy)
{
	if (Role == ROLE_Authority && TouchedBy->Controller != NULL && !((AUTCharacter*)TouchedBy)->IsRagdoll() && TouchedBy->PlayerState != NULL)
	{
		AUTPlayerState* PS = Cast<AUTPlayerState>(TouchedBy->PlayerState);
		if (PS != NULL)
		{
			if (ControllingPawn == NULL ||
				(PS != ControllingPawn) || (PS != ControllingPawn && PS->GetTeamNum() != GetControllingTeamNum())
				|| (PS == ControllingPawn && PS->GetTeamNum() != GetControllingTeamNum()))
			{
				ControllingPawn = PS;
				UpdateStatus();
			}
		}
	}
}

void AControlPoint::OnOverlapEnd_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role == ROLE_Authority && OtherActor != ControllingPawn)
	{
		TArray<AActor*> Touching;
		GetOverlappingActors(Touching, APawn::StaticClass());
		for (AActor* TouchingActor : Touching)
		{
			APawn* P = Cast<APawn>(TouchingActor);
			if (P != NULL && !((AUTCharacter*)P)->IsRagdoll() && P->PlayerState != NULL)
			{
				AUTPlayerState* PS = Cast<AUTPlayerState>(P->PlayerState);
				if (PS != NULL)
				{
					ControllingPawn = PS;
					UpdateStatus();
					break;
				}
			}
		}
	}
}

void AControlPoint::UpdateStatus()
{
	int32 newTeam = 255;
	if (ControllingPawn == NULL)
	{
		newTeam = 255;
	}
	else
	{
		newTeam = ControllingPawn->GetTeamNum();
	}

	if (newTeam == 255 || newTeam == GetControllingTeamNum())
	{
		return;
	}

	AUTDomGameState* GS = Cast<AUTDomGameState>(GetWorld()->GetGameState<AUTGameState>());
	ControllingTeamNum = newTeam;
	ControllingTeam = Cast<AUTDomTeamInfo>(ControllingPawn->Team);
	ControlledTime = GetWorld()->GetTimeSeconds();
	if (ControllingTeam == NULL || GetControllingTeamNum() == 4)
	{
		bScoreReady = false;
		if (GS)
		{
			GS->UpdateControlPointFX(this, 4);
		}
	}
	else
	{
		SetTeam(ControllingTeam);
		if (GS)
		{
			GS->UpdateControlPointFX(this, GetControllingTeamNum());
		}
		CarriedObjectHolder = ControllingPawn;
		if (ControlPointCaptureSound)
		{
			UUTGameplayStatics::UTPlaySound(GetWorld(), ControlPointCaptureSound, this);
		}
		if (Role == ROLE_Authority)
		{
			if (!GetWorldTimerManager().IsTimerActive(ScoreTimeNotifyHandle))
			{
				bScoreReady = false;
				GetWorldTimerManager().SetTimer(ScoreTimeNotifyHandle, this, &AControlPoint::ScoreTimeNotify, ScoreTime, false);
			}
		}
	}
	ControllingPawn->MakeNoise(2.0);
	AUTDomGameMode* GM = GetWorld()->GetAuthGameMode<AUTDomGameMode>();
	if (GM)
	{
		GM->BroadcastLocalized(this, MessageClass, GetControllingTeamNum(), NULL, NULL, this);
	}

	if (Role == ROLE_Authority)
	{
		if (GS != NULL)
		{
			for (AUTTeamInfo* Team : GS->Teams)
			{
				Team->NotifyObjectiveEvent(this, ControllingPawn->GetInstigatorController(), FName(TEXT("FlagStatusChange")));
			}
		}
	}
}

/**
 * @note this should not be called directly. Use AUTDomGameState->UpdateControlPointFX()
 * 		for changes to be replicated to clients.
 **/
void AControlPoint::UpdateTeamEffects_Implementation(int32 TeamIndex)
{
	if (TeamMesh.IsValidIndex(TeamIndex))
	{
		DomMesh->SetStaticMesh(TeamMesh[TeamIndex]);
	}
	if (DomLightColor.IsValidIndex(TeamIndex))
	{
		DomLight->SetLightColor(DomLightColor[TeamIndex]);
	}
	ForceNetUpdate();
}

void AControlPoint::ResetPoint(bool IsEnabled)
{
	Reset_Implementation();
	AUTDomGameState* GS = Cast<AUTDomGameState>(GetWorld()->GetGameState<AUTGameState>());
	if (GS)
	{
		if (IsEnabled)
		{
			ControllingTeamNum = 255;
			GS->UpdateControlPointFX(this, 4);
		}
		else
		{
			ControllingTeamNum = 5;
			GS->UpdateControlPointFX(this, 5);
		}
	}
}

void AControlPoint::Reset_Implementation()
{
	ControllingPawn = NULL;
	ControllingTeam = NULL;
	CarriedObjectHolder = NULL;
	ControllingTeamNum = 255;
	ControlledTime = GetWorld()->TimeSeconds;
	ForceNetUpdate();
}

void AControlPoint::DisablePoint()
{
	Disable_Implementation();
}

void AControlPoint::Disable_Implementation()
{
	ControllingPawn = NULL;
	ControllingTeam = NULL;
	CarriedObjectHolder = NULL;
	ControllingTeamNum = 255;
	DomCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DomMesh->SetVisibility(false);
	DomMesh->SetHiddenInGame(true);
	DomLight->SetVisibility(false);
	DomLight->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	ForceNetUpdate();
}

void AControlPoint::SetTeam(AUTTeamInfo* NewTeam)
{
	ControllingTeam = Cast<AUTDomTeamInfo>(NewTeam);
	if (ControllingTeamNum != NewTeam->GetTeamNum())
	{
		ControllingTeamNum = NewTeam->GetTeamNum();
	}
	ForceNetUpdate();
}

AUTPlayerState* AControlPoint::GetCarriedObjectHolder()
{
	return ControllingPawn;
}

void AControlPoint::ScoreTimeNotify()
{
	bScoreReady = true;
}
