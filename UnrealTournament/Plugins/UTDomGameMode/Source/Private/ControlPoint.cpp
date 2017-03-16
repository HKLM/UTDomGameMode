// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "Net/UnrealNetwork.h"
#include "CollisionQueryParams.h"
#include "UTDomGameMode.h"
#include "UTDomGameState.h"
#include "UTDoubleDomGameState.h"
#include "UTDomTeamInfo.h"
#include "UTDomStat.h"
#include "UTADomTypes.h"
#include "UTDomGameMessage.h"
#include "xDomPointA.h"
#include "xDomPointB.h"
#include "UTDomControlPoint.h"
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

	// Light
	DomLightColor.Insert(FLinearColor::Red, 0);
	DomLightColor.Insert(FLinearColor::Blue, 1);
	DomLightColor.Insert(FLinearColor::Green, 2);
	DomLightColor.Insert(FLinearColor::Yellow, 3);
	DomLightColor.Insert(FLinearColor::Gray, 4);
	DomLightColor.Insert(FLinearColor::FLinearColor(0.f, 0.f, 0.f, 0.f), 5);

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
	//--------Control Point----------------
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EditorControlPoint4Mesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/DomX.DomX'"));
	// StaticMesh
	EditorDomMesh = ObjectInitializer.CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>(this, FName(TEXT("EditorDomMesh")));
	EditorDomMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EditorDomMesh->SetStaticMesh(EditorControlPoint4Mesh.Object);
	EditorDomMesh->AlwaysLoadOnClient = true;
	EditorDomMesh->AlwaysLoadOnServer = true;
	EditorDomMesh->bCastDynamicShadow = true;
	EditorDomMesh->bAffectDynamicIndirectLighting = true;
	EditorDomMesh->bReceivesDecals = false;
	EditorDomMesh->SetupAttachment(RootComponent);
	EditorDomMesh->RelativeLocation.Z = 40;
	EditorDomMesh->bIsEditorOnly = true;

	// Spinner
	EditorMeshSpinner = ObjectInitializer.CreateEditorOnlyDefaultSubobject<URotatingMovementComponent>(this, FName(TEXT("EditorMeshSpinner")));
	EditorMeshSpinner->UpdatedComponent = EditorDomMesh;
	EditorMeshSpinner->RotationRate.Yaw = 80.0f;
	EditorMeshSpinner->bIsEditorOnly = true;

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

	ObjectiveType = EControlPoint::PT_ControlPoint;
	ScoreTime = 0.1f;
	//MessageClass = UUTDomGameMessage::StaticClass();
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
	DOREPLIFETIME(AControlPoint, ControllingPawn);
	DOREPLIFETIME(AControlPoint, ControllingTeam);
	DOREPLIFETIME_CONDITION(AControlPoint, bIsGameObjective, COND_None);
	DOREPLIFETIME(AControlPoint, ThisControlPoint);
	DOREPLIFETIME_CONDITION(AControlPoint, ObjectiveType, COND_None);
	DOREPLIFETIME_CONDITION(AControlPoint, PointName, COND_InitialOnly);
}

#if WITH_EDITOR
void AControlPoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ObjectiveType = EControlPoint::PT_ControlPoint;
	EditorDomMesh->SetHiddenInGame(true);
	EditorDomMesh->SetVisibility(true);
}
#endif

void AControlPoint::BeginPlay()
{
	Super::BeginPlay();
	SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, 15.0f));

	if (Role == ROLE_Authority)
	{
		//FTimerHandle TempHandle;
		GetWorldTimerManager().SetTimer(ControlledTimerHandle, this, &AControlPoint::TeamHeldTimer, 1.0f, true);
	}
}

void AControlPoint::CreateCarriedObject()
{
	AUTDoubleDomGameState* DDGS = Cast<AUTDoubleDomGameState>(GetWorld()->GetGameState<AUTGameState>());
	if (DDGS)
	{
		ObjectiveType = DDGS->GetControlPointType();
	}
	else
	{
		AUTDomGameState* GS = Cast<AUTDomGameState>(GetWorld()->GetGameState<AUTGameState>());
		if (GS)
		{
			ObjectiveType = GS->GetControlPointType();
		}
	}
	if (ObjectiveType == EControlPoint::PT_Disabled) return;

	FActorSpawnParameters Params;
	Params.Owner = this;
	if (ObjectiveType == EControlPoint::PT_ControlPoint)
	{
		ThisControlPoint = GetWorld()->SpawnActor<AUTDomControlPoint>(AUTDomControlPoint::StaticClass(), GetActorLocation(), GetActorRotation(), Params);
		ThisControlPoint->PointName = this->PointName;
	}
	else if (ObjectiveType == EControlPoint::PT_xDomA)
	{
		ThisControlPoint = GetWorld()->SpawnActor<AxDomPointA>(AxDomPointA::StaticClass(), GetActorLocation(), GetActorRotation(), Params);
		ThisControlPoint->PointName = TEXT("A");
	}
	else if (ObjectiveType == EControlPoint::PT_xDomB)
	{
		ThisControlPoint = GetWorld()->SpawnActor<AxDomPointB>(AxDomPointB::StaticClass(), GetActorLocation(), GetActorRotation(), Params);
		ThisControlPoint->PointName = TEXT("B");
	}

	if (ThisControlPoint != nullptr)
	{
		ThisControlPoint->Init(this);
		ThisControlPoint->SetReplicates(true);
		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
		ThisControlPoint->AttachToActor(this, AttachmentRules, NAME_None);
	}
}

void AControlPoint::TeamHeldTimer()
{
	if (Role == ROLE_Authority)
	{
		AUTDomGameState* GS = Cast<AUTDomGameState>(GetWorld()->GetGameState<AUTGameState>());
		
		if ((GS && (!GS->HasMatchStarted() || GS->HasMatchEnded())) || bStopControlledTimer) return;

		if (ControllingPawn != nullptr)
		{
			ControllingPawn->ModifyStatsValue(NAME_ControlPointHeldTime, 1.0f);
		}
		if (ControllingTeam != nullptr)
		{
			ControllingTeam->ModifyStatsValue(NAME_TeamControlPointHeldTime, 1.0f);
		}
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
	if (Role == ROLE_Authority)
	{
		AUTCharacter* Character = Cast<AUTCharacter>(OtherActor);
		if (Character != nullptr && !GetWorld()->LineTraceTestByChannel(OtherActor->GetActorLocation(), GetActorLocation(), ECC_Pawn, FCollisionQueryParams(), WorldResponseParams))
		{
			APawn* P = Cast<APawn>(OtherActor);
			if (P != nullptr)
			{
				ProcessTouch(Character);
			}
		}
	}
}

void AControlPoint::ProcessTouch_Implementation(APawn* TouchedBy)
{
	if (Role == ROLE_Authority
		&& bScoreReady
		&& TouchedBy->Controller != nullptr
		&& !((AUTCharacter*)TouchedBy)->IsRagdoll()
		&& TouchedBy->PlayerState != nullptr)
	{
		AUTPlayerState* PS = Cast<AUTPlayerState>(TouchedBy->PlayerState);
		if (PS != nullptr)
		{
			if (ControllingPawn == nullptr
				|| (PS != ControllingPawn)
				|| (PS != ControllingPawn && PS->Team->GetTeamNum() != GetTeamNum())
				|| (PS == ControllingPawn && PS->Team->GetTeamNum() != GetTeamNum()))
			{
				AUTDomGameState* GS = Cast<AUTDomGameState>(GetWorld()->GetGameState<AUTGameState>());
				if (GS != nullptr && (GS->IsMatchInProgress() && !GS->HasMatchEnded()))
				{
					ControllingPawn = PS;
					UpdateStatus();
				}
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
			if (P != nullptr
				&& !((AUTCharacter*)P)->IsRagdoll()
				&& P->PlayerState != nullptr)
			{
				AUTPlayerState* PS = Cast<AUTPlayerState>(P->PlayerState);
				if (PS != nullptr)
				{
					AUTDomGameState* GS = Cast<AUTDomGameState>(GetWorld()->GetGameState<AUTGameState>());
					if (GS != nullptr && (GS->IsMatchInProgress() && !GS->HasMatchEnded()))
					{
						ControllingPawn = PS;
						UpdateStatus();
						break;
					}
				}
			}
		}
	}
}

void AControlPoint::UpdateStatus()
{
	uint8 newTeam = 255;
	if (ControllingPawn == nullptr || (ControllingPawn	&& ControllingPawn->Team == nullptr))
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
	if (GS == nullptr) return;

	TeamNum = newTeam;
	ControllingTeam = Cast<AUTDomTeamInfo>(ControllingPawn->Team);
	ControlledTime = GetWorld()->GetTimeSeconds();
	// Neutral or no controlling team
	if (ControllingTeam == nullptr || TeamNum == 4)
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
 */
void AControlPoint::UpdateTeamEffects_Implementation(uint8 TeamIndex)
{
	ForceNetUpdate();
}

void AControlPoint::ResetPoint(bool IsEnabled)
{
	if (IsEnabled)
	{
		//Reset to Neutral
		TeamNum = 255;
	}
	else
	{
		//Disable
		TeamNum = 5;
	}
	Reset();
}

void AControlPoint::Reset_Implementation()
{
	ControllingPawn = nullptr;
	ControllingTeam = nullptr;
	CarriedObjectHolder = nullptr;
	ControlledTime = GetWorld()->TimeSeconds;
	GetWorldTimerManager().ClearTimer(ControlledTimerHandle);
	AUTDomGameState* GS = Cast<AUTDomGameState>(GetWorld()->GetGameState<AUTGameState>());
	if (GS && TeamNum == 5)
	{
		//Disable
		GS->UpdateControlPointFX(this, 5);
	}
	else
	{
		TeamNum = 255;
		if (GS)
		{
			//Reset to Neutral
			GS->UpdateControlPointFX(this, 4);
		}
	}
	ForceNetUpdate();
}

void AControlPoint::DisablePoint()
{
	Disable_Implementation();
}

void AControlPoint::Disable_Implementation()
{
	ControllingPawn = nullptr;
	ControllingTeam = nullptr;
	CarriedObjectHolder = nullptr;
	TeamNum = 5;
	DomCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DomLight->SetVisibility(false);
	DomLight->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	ForceNetUpdate();
}

uint8 AControlPoint::GetTeamNum() const
{
	if (ControllingTeam != nullptr)
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
	if (Role == ROLE_Authority && thePlayer != nullptr)
	{
		AUTDomGameState* GS = Cast<AUTDomGameState>(GetWorld()->GetGameState<AUTGameState>());
		if (GS && GS->HasMatchStarted() && !GS->HasMatchEnded())
		{
			thePlayer->ModifyStatsValue(NAME_ControlPointHeldPoints, ScoreAmmount);
		}
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
