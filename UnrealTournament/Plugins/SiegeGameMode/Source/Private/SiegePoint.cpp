// Created by Brian 'Snake' Alexander, (c) 2016
#include "UnrealTournament.h"
#include "Net/UnrealNetwork.h"
#include "CollisionQueryParams.h"
#include "SiegeGameMode.h"
#include "SiegeGameState.h"
//#include "UTDomTeamInfo.h"
#include "UTDomStat.h"
#include "UTADomTypes.h"
#include "SiegeGameMessage.h"
#include "SiegePoint.h"

FCollisionResponseParams WorldResponseParams = []()
{
	FCollisionResponseParams Result(ECR_Ignore);
	Result.CollisionResponse.WorldStatic = ECR_Block;
	Result.CollisionResponse.WorldDynamic = ECR_Block;
	return Result;
}();

ASiegePoint::ASiegePoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Collision
	Collision->InitCapsuleSize(90.0f, 140.0f);
	Collision->RelativeLocation.Z = 5.0f;
	Collision->OnComponentBeginOverlap.Clear();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASiegePoint::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ASiegePoint::OnOverlapEnd_Implementation);


	MovementComponent->SetActive(false, false);
	MovementComponent->DestroyComponent();

	// StaticMesh assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SiegePoint0Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomR.DomR'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SiegePoint1Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomB.DomB'"));
	TeamMesh.Insert(SiegePoint0Mesh.Object, 0);
	TeamMesh.Insert(SiegePoint1Mesh.Object, 1);


	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> SiegePoint0Skin(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/Skin0_Mat_Inst.Skin0_Mat_Inst'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> SiegePoint1Skin(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/Skin1_Mat_Inst.Skin1_Mat_Inst'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> SiegePoint4Skin(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/Skin4_Mat_Inst.Skin4_Mat_Inst'"));
	TeamSkins.Insert(SiegePoint0Skin.Object, 0);
	TeamSkins.Insert(SiegePoint1Skin.Object, 1);
	TeamSkins.Insert(SiegePoint4Skin.Object, 2);
	TeamSkins.Insert(NULL, 3);
	TeamSkins.Insert(SiegePoint4Skin.Object, 4);

	// StaticMesh
	DomMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FName(TEXT("Mesh")));
	DomMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DomMesh->SetStaticMesh(SiegePoint0Mesh.Object);
	DomMesh->SetMaterial(0, TeamSkins[2]);
	DomMesh->AlwaysLoadOnClient = true;
	DomMesh->AlwaysLoadOnServer = true;
	DomMesh->bCastDynamicShadow = true;
	DomMesh->bAffectDynamicIndirectLighting = true;
	DomMesh->bReceivesDecals = false;
	DomMesh->AttachParent = RootComponent;
	DomMesh->RelativeLocation.Z = -50;

	// Spinner
	MeshSpinner = ObjectInitializer.CreateDefaultSubobject<URotatingMovementComponent>(this, FName(TEXT("MeshSpinner")));
	MeshSpinner->UpdatedComponent = DomMesh;
	MeshSpinner->RotationRate.Yaw = 80.0f;

	// Light
	DomLightColor.Insert(FLinearColor::Red, 0);
	DomLightColor.Insert(FLinearColor::Blue, 1);
	DomLightColor.Insert(FLinearColor::Gray, 2);
	DomLightColor.Insert(FLinearColor::Black, 3);
	DomLightColor.Insert(FLinearColor::Gray, 4);
	DomLight = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, FName(TEXT("Light")));
	DomLight->AttachParent = RootComponent;
	DomLight->SetMobility(EComponentMobility::Movable);
	DomLight->RelativeLocation.Z = 20.0f;
	DomLight->SetAttenuationRadius(940.0f);
	DomLight->bUseInverseSquaredFalloff = false;
	DomLight->SetAffectDynamicIndirectLighting(true);
	DomLight->SetIntensity(10.0f);
	DomLight->SetLightColor(DomLightColor[2]);

	static ConstructorHelpers::FObjectFinder<USoundBase> CaptureSound(TEXT("SoundCue'/UTDomGameMode/UTDomGameContent/Sounds/ControlSound_Cue.ControlSound_Cue'"));
	SiegePointCaptureSound = CaptureSound.Object;

	ScoreTime = 0.1f;
	MessageClass = USiegeGameMessage::StaticClass();
	SetReplicates(true);
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0;
	bScoreReady = true;
	bStopControlledTimer = false;
}

void ASiegePoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASiegePoint, DomMesh);
	DOREPLIFETIME(ASiegePoint, ControllingPawn);
	DOREPLIFETIME(ASiegePoint, ControllingTeam);
	DOREPLIFETIME_CONDITION(ASiegePoint, BaseTeam, COND_InitialOnly);
}

void ASiegePoint::BeginPlay()
{
	Super::BeginPlay();
	SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, 15.0f));

	if (Role == ROLE_Authority)
	{
		FTimerHandle TempHandle;
		GetWorldTimerManager().SetTimer(TempHandle, this, &ASiegePoint::TeamHeldTimer, 1.0f, true);
	}
}

void ASiegePoint::OnConstruction(const FTransform& Transform)
{
	Super::AActor::OnConstruction(Transform);

	// backwards compatibility; force values on existing instances
	Collision->SetAbsolute(false, false, true);
	if (Role == ROLE_Authority)
	{
		//Collision->SetWorldRotation(FRotator(0.0f, 0.f, 0.f));
		//GetWorldTimerManager().SetTimer(CheckTouchingHandle, this, &AUTCarriedObject::CheckTouching, 0.05f, false);
	}
}

void ASiegePoint::Init(AUTGameObjective* NewBase)
{
	// Look up the team for my CarriedObject
	uint8 DesiredTeamNum = NewBase->GetTeamNum();
	if (DesiredTeamNum < 255)
	{
		ASiegeGameState* GameState = GetWorld()->GetGameState<ASiegeGameState>();
		if (GameState != NULL && DesiredTeamNum < GameState->Teams.Num())
		{
			Team = GameState->Teams[DesiredTeamNum];
		}
	}
	switch (DesiredTeamNum)
	{
		case 0:
			BaseTeam = EBaseTeam::Team_Red; break;
		case 1:
			BaseTeam = EBaseTeam::Team_Blue; break;
		default:
			break;
	}
	DomMesh->SetStaticMesh(TeamMesh[DesiredTeamNum]);
	DomMesh->SetMaterial(0, TeamSkins[2]);
	DomLight->SetLightColor(DomLightColor[2]);

	bInitialized = true;
	HomeBase = NewBase;
	ObjectState = CarriedObjectState::Home;
}

void ASiegePoint::TeamHeldTimer()
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

FString ASiegePoint::GetPointName() const
{
	return PointName;
}

AUTPlayerState* ASiegePoint::GetSiegePointHolder() const
{
	return ControllingPawn;
}

void ASiegePoint::OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ASiegePoint::ProcessTouch_Implementation(APawn* TouchedBy)
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

void ASiegePoint::OnOverlapEnd_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void ASiegePoint::UpdateStatus()
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

	ASiegeGameState* GS = Cast<ASiegeGameState>(GetWorld()->GetGameState<AUTGameState>());
	if (GS == NULL) return;

	Team = ControllingPawn->Team;
	ControllingTeam = Cast<AUTDomTeamInfo>(ControllingPawn->Team);
	ControlledTime = GetWorld()->GetTimeSeconds();
	// Neutral or no controlling team
	if (ControllingTeam == NULL || Team == NULL)
	{
		GS->UpdateSiegePointFX(this, 4);
	}
	else
	{
		bScoreReady = false;
		GS->UpdateSiegePointFX(this, ControllingTeam->GetTeamNum());
		Holder = ControllingPawn;
		if (SiegePointCaptureSound)
		{
			UUTGameplayStatics::UTPlaySound(GetWorld(), SiegePointCaptureSound, this);
		}
		ControllingPawn->MakeNoise(2.0);
		if (Role == ROLE_Authority)
		{
			ASiegeGameMode* GM = GetWorld()->GetAuthGameMode<ASiegeGameMode>();
			if (GM)
			{
				// Update stats
				ControllingPawn->ModifyStatsValue(NAME_ControlPointCaps, 1);
				// Send HUD message
				GM->BroadcastLocalized(this, MessageClass, ControllingPawn->GetTeamNum(), NULL, NULL, this);
			}
			GetWorldTimerManager().SetTimer(ScoreTimeNotifyHandle, this, &ASiegePoint::ScoreTimeNotify, ScoreTime, false);
			for (AUTTeamInfo* Team : GS->Teams)
			{
				Team->NotifyObjectiveEvent(this, ControllingPawn->GetInstigatorController(), FName(TEXT("NewControllingTeam")));
			}
		}
	}
}

/**
 * @note this should not be called directly. Use ASiegeGameState->UpdateSiegePointFX()
 * 		 for changes to be replicated to clients.
 **/
void ASiegePoint::UpdateTeamEffects_Implementation(uint8 TeamIndex)
{
	if (TeamIndex > 2)
	{
		TeamIndex = 2;
	}
	if (TeamSkins.IsValidIndex(TeamIndex))
	{
		DomMesh->SetMaterial(0, TeamSkins[TeamIndex]);
	}
	if (DomLightColor.IsValidIndex(TeamIndex))
	{
		DomLight->SetLightColor(DomLightColor[TeamIndex]);
	}
	ForceNetUpdate();
}

void ASiegePoint::ResetPoint(bool IsEnabled)
{
	Reset_Implementation();
	ASiegeGameState* GS = Cast<ASiegeGameState>(GetWorld()->GetGameState<AUTGameState>());
	if (GS)
	{
		if (IsEnabled)
		{
			//Reset to Neutral
			GS->UpdateSiegePointFX(this, 4);
		}
		else
		{
			//Disable
			GS->UpdateSiegePointFX(this, 5);
		}
	}
}

void ASiegePoint::Reset_Implementation()
{
	ControllingPawn = NULL;
	ControllingTeam = NULL;
	Holder = NULL;
	HoldingPawn = NULL;
	Team = NULL;
	ControlledTime = GetWorld()->TimeSeconds;
	ForceNetUpdate();
}

void ASiegePoint::DisablePoint()
{
	Disable_Implementation();
}

void ASiegePoint::Disable_Implementation()
{
	ControllingPawn = NULL;
	ControllingTeam = NULL;
	Holder = NULL;
	HoldingPawn = NULL;
	Team = NULL;
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DomMesh->SetVisibility(false);
	DomMesh->SetHiddenInGame(true);
	DomLight->SetVisibility(false);
	DomLight->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	ForceNetUpdate();
}

uint8 ASiegePoint::GetTeamNum() const
{
	if (ControllingTeam != NULL)
	{
		return ControllingTeam->TeamIndex;
	}
	else
	{
		return 4;
	}
}

void ASiegePoint::ScoreTimeNotify()
{
	bScoreReady = true;
}

void ASiegePoint::UpdateHeldPointStat(AUTPlayerState* thePlayer, float ScoreAmmount)
{
	if (Role == ROLE_Authority && thePlayer != NULL)
	{
		thePlayer->ModifyStatsValue(NAME_ControlPointHeldPoints, ScoreAmmount);
	}
}

TEnumAsByte<EControllingTeam::Type> ASiegePoint::NotifyTeamChanged() const
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
