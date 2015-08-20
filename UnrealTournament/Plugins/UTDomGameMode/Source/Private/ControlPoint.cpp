// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTDomGameState.h"
#include "UTDomGameMode.h"
#include "UTDomGameMessage.h"
#include "Net/UnrealNetwork.h"
#include "CollisionQueryParams.h"
#include "UTADomTypes.h"
#include "UTCarriedObject.h"
#include "BaseControlPoint.h"
#include "ControlPoint.h"
#include "UTRecastNavMesh.h"
#include "UTDomTeamInfo.h"

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
	bIgnoresOriginShifting = true;
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

	// RootComponent
	RootComponent = Collision;
	Collision->InitCapsuleSize(90.0f, 160.0f);
	Collision->SetCollisionProfileName(FName(TEXT("Pickup")));
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	Collision->OnComponentBeginOverlap.Clear();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AControlPoint::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AControlPoint::OnOverlapEnd_Implementation);

	// Mesh
	DomMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FName(TEXT("ControlPointBase")));
	DomMesh->AttachParent = RootComponent;
	DomMesh->SetNetAddressable();
	DomMesh->SetIsReplicated(true);
	DomMesh->SetEnableGravity(false);
	DomMesh->AlwaysLoadOnClient = true;
	DomMesh->AlwaysLoadOnServer = true;
	DomMesh->bReceivesDecals = false;
	DomMesh->SetMobility(EComponentMobility::Movable);
	DomMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DomMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
	DomMesh->SetStaticMesh(ControlPoint4Mesh.Object);
	DomMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));

	// Spinner
	MeshSpinner = ObjectInitializer.CreateDefaultSubobject<URotatingMovementComponent>(this, FName(TEXT("MeshSpinner")));
	MeshSpinner->UpdatedComponent = DomMesh;
	MeshSpinner->RotationRate.Yaw = 80.0f;

	// Light
	DomLight = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, FName(TEXT("Light")));
	DomLight->AttachParent = RootComponent;
	DomLight->SetAttenuationRadius(900.0f);
	DomLight->bUseInverseSquaredFalloff = false;
	DomLight->SetAffectDynamicIndirectLighting(true);
	DomLight->SetIntensity(10.0f);
	DomLight->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	DomLight->SetLightColor(DomLightColor[4]);

	ScoreTime = 2.0f;
	MessageClass = UUTDomGameMessage::StaticClass();
	bReplicates = true;
	SetReplicates(true);
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	ControllingTeamNum = 255;
	NetPriority = 3.0;
	MyObjectiveType = EDomObjectiveType::ControlPoint;
}

void AControlPoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AControlPoint, MyObjectiveType, COND_InitialOnly);
	DOREPLIFETIME(AControlPoint, DomMesh);
	DOREPLIFETIME(AControlPoint, ScoreTime);
}

void AControlPoint::Init(AUTGameObjective* NewBase)
{
	//Super::Init(NewBase);
	UpdateTeamEffects(4);
	HomeBase = NewBase;
}

void AControlPoint::BeginPlay()
{
	Super::BeginPlay();
	AUTRecastNavMesh* NavData = GetUTNavData(GetWorld());
	if (NavData != NULL)
	{
		NavData->AddToNavigation(this);
	}
}

void AControlPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
	AUTRecastNavMesh* NavData = GetUTNavData(GetWorld());
	if (NavData != NULL)
	{
		NavData->RemoveFromNavigation(this);
	}
}

AControlPoint* AControlPoint::GetControlPoint()
{
	return this;
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
	if (MyObjectiveType == EDomObjectiveType::xPoint)
	{
		UpdateStatus_xPoint();
	}
	else if (MyObjectiveType == EDomObjectiveType::Disabled)
	{
		DisablePoint();
	}
	else
	{
		UpdateStatus_ControlPoint();
	}
}

void AControlPoint::UpdateStatus_ControlPoint()
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
		if (GS){
			GS->UpdateControlPointFX(this, 4);
		}
	}
	else
	{
		SetTeam(ControllingTeam);
		if (GS){
			GS->UpdateControlPointFX(this, GetControllingTeamNum());
		}
		SetHolder(ControllingPawn->GetUTCharacter());

		if (ControlPointCaptureSound){
			UUTGameplayStatics::UTPlaySound(GetWorld(), ControlPointCaptureSound, this);
		}
		if (Role == ROLE_Authority)
		{
			if (!GetWorldTimerManager().IsTimerActive(ScoreTimeNotifyHandle))
			{
				bScoreReady = false;
				GetWorldTimerManager().SetTimer(ScoreTimeNotifyHandle, this, &AControlPoint::SendHomeWithNotify, ScoreTime, false);
			}
		}
	}
}

void AControlPoint::UpdateStatus_xPoint() {}

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
	LastHoldingPawn = HoldingPawn;
	int32 AssistIndex = FindAssist(ControllingPawn);
	float LastHeldTime = GetWorld()->GetTimeSeconds() - PickedUpTime;
	if (AssistIndex >= 0 && AssistIndex < AssistTracking.Num())
	{
		AssistTracking[AssistIndex].TotalHeldTime += LastHeldTime;
	}
	ControllingPawn = NULL;
	ControllingTeam = NULL;
	ControlledTime = GetWorld()->TimeSeconds;
	ForceNetUpdate();
}

void AControlPoint::DisablePoint()
{
	Disable_Implementation();
}

void AControlPoint::Disable_Implementation()
{
	MyObjectiveType = EDomObjectiveType::Disabled;
	ControllingPawn = NULL;
	ControllingTeam = NULL;
	ControllingTeamNum = 255;
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DomMesh->SetVisibility(false);
	DomMesh->SetHiddenInGame(true);
	DomLight->SetVisibility(false);
	DomLight->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	ForceNetUpdate();
}

FName AControlPoint::GetTeamEventName(int32 TeamID) const
{
	switch (TeamID)
	{
		case 0:
			return CarriedObjectState::Red;
			break;
		case 1:
			return CarriedObjectState::Blue;
			break;
		case 2:
			return CarriedObjectState::Green;
			break;
		case 3:
			return CarriedObjectState::Gold;
			break;
		default:
			return CarriedObjectState::Neutral;
			break;
	}
}

void AControlPoint::SetHolder(AUTCharacter* NewHolder)
{
	// Sanity Checks
	if (NewHolder == NULL || NewHolder->bPendingKillPending || NewHolder->PlayerState == NULL || Cast<AUTPlayerState>(NewHolder->PlayerState) == NULL)
	{
		return;
	}
	ChangeState(GetTeamEventName(GetControllingTeamNum()));
	HoldingPawn = NewHolder;
	Holder = Cast<AUTPlayerState>(HoldingPawn->PlayerState);
	PickedUpTime = GetWorld()->GetTimeSeconds();
	if (Role == ROLE_Authority)
	{
		OnHolderChanged();
		//if (Holder)
		//{
		//	Holder->ModifyStatsValue(NAME_DomPointCaptures, 1);
		//	if (Holder->Team)
		//	{
		//		Holder->Team->ModifyStatsValue(NAME_TeamDomPointCaptures, 1);
		//	}
		//}
	}

	// Track the pawns that have held this flag - used for 
	//TODO$ Is this even needed?
	int32 AssistIndex = FindAssist(Holder);
	if (AssistIndex < 0)
	{
		FAssistTracker NewAssist;
		NewAssist.Holder = Holder;
		NewAssist.TotalHeldTime = 0.0f;
		AssistIndex = AssistTracking.Add(NewAssist);
	}

	HoldingPawn->MakeNoise(2.0);
	AUTDomGameMode* GM = GetWorld()->GetAuthGameMode<AUTDomGameMode>();
	if (GM)
	{
		GM->BroadcastLocalized(this, MessageClass, GetControllingTeamNum(), NULL, NULL, this);
	}

	if (Role == ROLE_Authority)
	{
		AUTGameState* GS = GetWorld()->GetGameState<AUTGameState>();
		if (GS != NULL)
		{
			for (AUTTeamInfo* Team : GS->Teams)
			{
				Team->NotifyObjectiveEvent(HomeBase, NewHolder->Controller, FName(TEXT("FlagStatusChange")));
			}
		}
	}
}

void AControlPoint::ChangeState(FName NewCarriedObjectState)
{
	if (Role == ROLE_Authority)
	{
		ObjectState = NewCarriedObjectState;
		OnObjectStateChanged();
		HomeBase->ObjectStateWasChanged(ObjectState);
	}
}

void AControlPoint::OnObjectStateChanged()
{
	OnCarriedObjectStateChangedDelegate.Broadcast(this, ObjectState);
}

void AControlPoint::OnHolderChanged()
{
	OnCarriedObjectHolderChangedDelegate.Broadcast(this);
}

void AControlPoint::SendHomeWithNotify()
{
	bScoreReady = true;
}
