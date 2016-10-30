// Created by Brian 'Snake' Alexander, 2015
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
	// Collision
	DomCollision = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("Collision"));
	DomCollision->InitCapsuleSize(90.0f, 140.0f);
	DomCollision->SetCollisionProfileName(FName(TEXT("Pickup")));
	DomCollision->RelativeLocation.Z = 140.0f;
	DomCollision->OnComponentBeginOverlap.AddDynamic(this, &AControlPoint::OnOverlapBegin);
	DomCollision->OnComponentEndOverlap.AddDynamic(this, &AControlPoint::OnOverlapEnd);
	DomCollision->SetupAttachment(RootComponent);

	// StaticMesh assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint0Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomR.DomR'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint1Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomB.DomB'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint2Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomGN.DomGN'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint3Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomGold.DomGold'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPoint4Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomX.DomX'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControlPointNullMesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomNULL.DomNULL'"));

	TeamMesh.Insert(ControlPoint0Mesh.Object, 0);
	TeamMesh.Insert(ControlPoint1Mesh.Object, 1);
	TeamMesh.Insert(ControlPoint2Mesh.Object, 2);
	TeamMesh.Insert(ControlPoint3Mesh.Object, 3);
	TeamMesh.Insert(ControlPoint4Mesh.Object, 4);
	TeamNullMesh = ControlPointNullMesh.Object;

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
	DomMesh->RelativeLocation.Z = 40;

	// Spinner
	MeshSpinner = ObjectInitializer.CreateDefaultSubobject<URotatingMovementComponent>(this, FName(TEXT("MeshSpinner")));
	MeshSpinner->UpdatedComponent = DomMesh;
	MeshSpinner->RotationRate.Yaw = 80.0f;

	// Light
	DomLightColor.Insert(FLinearColor::Red, 0);
	DomLightColor.Insert(FLinearColor::Blue, 1);
	DomLightColor.Insert(FLinearColor::Green, 2);
	DomLightColor.Insert(FLinearColor::Yellow, 3);
	DomLightColor.Insert(FLinearColor::Gray, 4);
	DomLight = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, FName(TEXT("Light")));
	DomLight->SetupAttachment(RootComponent);
	DomLight->RelativeLocation.Z = 90.0f;
	DomLight->SetAttenuationRadius(900.0f);
	DomLight->bUseInverseSquaredFalloff = false;
	DomLight->SetAffectDynamicIndirectLighting(true);
	DomLight->SetIntensity(10.0f);
	DomLight->SetLightColor(DomLightColor[4]);

	static ConstructorHelpers::FObjectFinder<USoundBase> CaptureSound(TEXT("SoundCue'/UTDomGameMode/UTDomGameContent/Sounds/ControlSound_Cue.ControlSound_Cue'"));
	ControlPointCaptureSound = CaptureSound.Object;

#if WITH_EDITORONLY_DATA
	EditorSprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(FName(TEXT("EditorSprite")));
	if (EditorSprite != NULL)
	{
		EditorSprite->SetupAttachment(RootComponent);
		if (!IsRunningCommandlet())
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> SpriteObj(TEXT("/Game/RestrictedAssets/EditorAssets/Icons/generic_objective.generic_objective"));
			EditorSprite->Sprite = SpriteObj.Get();
			if (EditorSprite->Sprite != NULL)
			{
				EditorSprite->UL = EditorSprite->Sprite->GetSurfaceWidth();
				EditorSprite->VL = EditorSprite->Sprite->GetSurfaceHeight();
			}
		}
	}
#endif

	ScoreTime = 0.1f;
	MessageClass = UUTDomGameMessage::StaticClass();
	TeamNum = 255;
	SetReplicates(true);
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0;
	bScoreReady = true;
	bStopControlledTimer = false;
}

void AControlPoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AControlPoint, DomMesh);
	DOREPLIFETIME(AControlPoint, ControllingPawn);
	DOREPLIFETIME(AControlPoint, ControllingTeam);
}

void AControlPoint::BeginPlay()
{
	Super::BeginPlay();
	SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, 15.0f));

	if (Role == ROLE_Authority)
	{
		FTimerHandle TempHandle;
		GetWorldTimerManager().SetTimer(TempHandle, this, &AControlPoint::TeamHeldTimer, 1.0f, true);
	}
}

void AControlPoint::CreateCarriedObject()
{
	//Optimize game resources. Dont need green and gold team meshes if playing 2 team mode.
	AUTDomGameState* GS = Cast<AUTDomGameState>(GetWorld()->GetGameState<AUTGameState>());
	if (GS != NULL)
	{
		uint8 TotalTeams = GS->NumTeams;
		if (TotalTeams == 2)
		{
			TeamMesh[2] = TeamNullMesh;
			TeamMesh[3] = TeamNullMesh;
		}
		else if (TotalTeams == 3)
		{
			TeamMesh[3] = TeamNullMesh;
		}
		else if (TotalTeams == 4)
		{
			TeamNullMesh->ReleaseResources();
		}
	}
}

void AControlPoint::TeamHeldTimer()
{
	if (bStopControlledTimer) return;

	if (!bHidden && ControllingPawn != NULL)
	{
		ControllingPawn->ModifyStatsValue(NAME_ControlPointHeldTime, 1.0f);
	}
	if (!bHidden && ControllingTeam != NULL)
	{
		ControllingTeam->ModifyStatsValue(NAME_TeamControlPointHeldTime, 1.0f);
	}
}

FString AControlPoint::GetPointName() const
{
	return PointName;
}

AUTPlayerState* AControlPoint::GetControlPointHolder() const
{
	return ControllingPawn;
}

AUTPlayerState* AControlPoint::GetCarriedObjectHolder()
{
	return ControllingPawn;
}

void AControlPoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
	if (Role == ROLE_Authority
		&& bScoreReady
		&& TouchedBy->Controller != NULL
		&& !((AUTCharacter*)TouchedBy)->IsRagdoll()
		&& TouchedBy->PlayerState != NULL)
	{
		AUTPlayerState* PS = Cast<AUTPlayerState>(TouchedBy->PlayerState);
		if (PS != NULL)
		{
			if (ControllingPawn == NULL
				|| (PS != ControllingPawn)
				|| (PS != ControllingPawn && PS->Team->GetTeamNum() != GetTeamNum())
				|| (PS == ControllingPawn && PS->Team->GetTeamNum() != GetTeamNum()))
			{
				ControllingPawn = PS;
				UpdateStatus();
			}
		}
	}
}

void AControlPoint::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role == ROLE_Authority && bScoreReady && OtherActor != ControllingPawn)
	{
		TArray<AActor*> Touching;
		GetOverlappingActors(Touching, APawn::StaticClass());
		for (AActor* TouchingActor : Touching)
		{
			APawn* P = Cast<APawn>(TouchingActor);
			if (P != NULL
				&& !((AUTCharacter*)P)->IsRagdoll()
				&& P->PlayerState != NULL)
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
	uint8 newTeam = 255;
	if (ControllingPawn == NULL || (ControllingPawn	&& ControllingPawn->Team == NULL))
	{
		newTeam = 255;
	}
	else
	{
		newTeam = ControllingPawn->Team->GetTeamNum();
	}

	if (newTeam == 255 || newTeam == GetTeamNum())
	{
		return;
	}

	AUTDomGameState* GS = Cast<AUTDomGameState>(GetWorld()->GetGameState<AUTGameState>());
	if (GS == NULL) return;

	TeamNum = newTeam;
	ControllingTeam = Cast<AUTDomTeamInfo>(ControllingPawn->Team);
	ControlledTime = GetWorld()->GetTimeSeconds();
	// Neutral or no controlling team
	if (ControllingTeam == NULL || TeamNum == 4)
	{
		GS->UpdateControlPointFX(this, 4);
	}
	else
	{
		bScoreReady = false;
		GS->UpdateControlPointFX(this, TeamNum);
		CarriedObjectHolder = ControllingPawn;
		if (ControlPointCaptureSound)
		{
			UUTGameplayStatics::UTPlaySound(GetWorld(), ControlPointCaptureSound, this);
		}
		ControllingPawn->MakeNoise(2.0);
		if (Role == ROLE_Authority)
		{
			AUTDomGameMode* GM = GetWorld()->GetAuthGameMode<AUTDomGameMode>();
			if (GM)
			{
				// Update stats
				ControllingPawn->ModifyStatsValue(NAME_ControlPointCaps, 1);
				// Send HUD message
				GM->BroadcastLocalized(this, MessageClass, ControllingPawn->GetTeamNum(), NULL, NULL, this);
			}
			GetWorldTimerManager().SetTimer(ScoreTimeNotifyHandle, this, &AControlPoint::ScoreTimeNotify, ScoreTime, false);
			for (AUTTeamInfo* Team : GS->Teams)
			{
				Team->NotifyObjectiveEvent(this, ControllingPawn->GetInstigatorController(), FName(TEXT("NewControllingTeam")));
			}
		}
	}
}

/**
 * @note this should not be called directly. Use AUTDomGameState->UpdateControlPointFX()
 * 		 for changes to be replicated to clients.
 **/
void AControlPoint::UpdateTeamEffects_Implementation(uint8 TeamIndex)
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
			//Reset to Neutral
			TeamNum = 255;
			GS->UpdateControlPointFX(this, 4);
		}
		else
		{
			//Disable
			TeamNum = 5;
			GS->UpdateControlPointFX(this, 5);
		}
	}
}

void AControlPoint::Reset_Implementation()
{
	ControllingPawn = NULL;
	ControllingTeam = NULL;
	CarriedObjectHolder = NULL;
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
	TeamNum = 5;
	DomCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DomMesh->SetVisibility(false);
	DomMesh->SetHiddenInGame(true);
	DomLight->SetVisibility(false);
	DomLight->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	ForceNetUpdate();
}

uint8 AControlPoint::GetTeamNum() const
{
	if (ControllingTeam != NULL)
	{
		return ControllingTeam->TeamIndex;
	}
	else
	{
		return (TeamNum >= 6) ? 4 : TeamNum;
	}
}

void AControlPoint::ScoreTimeNotify()
{
	bScoreReady = true;
}

void AControlPoint::UpdateHeldPointStat(AUTPlayerState* thePlayer, float ScoreAmmount)
{
	if (Role == ROLE_Authority && thePlayer != NULL)
	{
		thePlayer->ModifyStatsValue(NAME_ControlPointHeldPoints, ScoreAmmount);
	}
}

TEnumAsByte<EControllingTeam::Type> AControlPoint::NotifyTeamChanged() const
{
	switch (this->GetTeamNum())
	{
	case 0: return EControllingTeam::TE_Red; break;
	case 1: return EControllingTeam::TE_Blue; break;
	case 2: return EControllingTeam::TE_Green; break;
	case 3: return EControllingTeam::TE_Gold; break;
	case 5: return EControllingTeam::TE_Disabled; break;
	default: return EControllingTeam::TE_Neutral; break;
	}
}
