// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "UTADomTypes.h"
#include "ControlPoint.h"
#include "UTDoubleDomGameMessage.h"
#include "UTDoubleDomGameState.h"
#include "UTDoubleDomGameMode.h"
#include "xDomPoint.h"

AxDomPoint::AxDomPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Base Mesh - materials
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DDomBaseMesh(TEXT("StaticMesh'/Game/RestrictedAssets/UTDomGameContent/Meshes/DDomPointBase.DDomPointBase'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex0(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Textures/DominationPointTex_Mat_Inst0.DominationPointTex_Mat_Inst0'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex1(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Textures/DominationPointTex_Mat_Inst1.DominationPointTex_Mat_Inst1'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex2(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Textures/DominationPointTex_Mat_Inst2.DominationPointTex_Mat_Inst2'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex3(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Textures/DominationPointTex_Mat_Inst3.DominationPointTex_Mat_Inst3'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex4(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Textures/DominationPointTex_Mat_Inst4.DominationPointTex_Mat_Inst4'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex5(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Textures/DominationPointTex_Mat_Inst5.DominationPointTex_Mat_Inst5'"));
	BaseMeshSkin.Insert(BTex0.Object, 0);
	BaseMeshSkin.Insert(BTex1.Object, 1);
	BaseMeshSkin.Insert(BTex2.Object, 2);
	BaseMeshSkin.Insert(BTex3.Object, 3);
	BaseMeshSkin.Insert(BTex4.Object, 4);
	BaseMeshSkin.Insert(BTex5.Object, 5);
	// Base Mesh
	if (DomMesh != NULL)
	{
		DomMesh->SetStaticMesh(DDomBaseMesh.Object);
		DomMesh->SetMaterial(0, BaseMeshSkin[4]);
		DomMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -95.0f));
	}

	if (Collision != NULL)
	{
		Collision->InitCapsuleSize(160.0f, 180.0f);
		Collision->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
	}

	//DomLetter - Skins
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LTex0(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Textures/DOMPointLetter_Mat_Inst0.DOMPointLetter_Mat_Inst0'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LTex1(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Textures/DOMPointLetter_Mat_Inst1.DOMPointLetter_Mat_Inst1'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LTex2(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Textures/DOMPointLetter_Mat_Inst2.DOMPointLetter_Mat_Inst2'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LTex3(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Textures/DOMPointLetter_Mat_Inst3.DOMPointLetter_Mat_Inst3'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LTex4(TEXT("MaterialInstanceConstant'/Game/RestrictedAssets/UTDomGameContent/Textures/DOMPointLetter_Mat_Inst4.DOMPointLetter_Mat_Inst4'"));
	LetterMeshSkins.Insert(LTex0.Object, 0);
	LetterMeshSkins.Insert(LTex1.Object, 1);
	LetterMeshSkins.Insert(LTex2.Object, 2);
	LetterMeshSkins.Insert(LTex3.Object, 3);
	LetterMeshSkins.Insert(LTex4.Object, 4);
	//DomLetter - Staticmeshes
	static ConstructorHelpers::FObjectFinder<UStaticMesh> AMesh(TEXT("StaticMesh'/Game/RestrictedAssets/UTDomGameContent/Meshes/DDomA.DDomA'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BMesh(TEXT("StaticMesh'/Game/RestrictedAssets/UTDomGameContent/Meshes/DDomB.DDomB'"));
	TeamMesh.Empty(0);
	TeamMesh.Insert(AMesh.Object, 0);
	TeamMesh.Insert(BMesh.Object, 1);

	LetterMesh = ObjectInitializer.CreateOptionalDefaultSubobject<UStaticMeshComponent>(this, FName(TEXT("xPointBase")));
	LetterMesh->SetStaticMesh(TeamMesh[0]);
	LetterMesh->SetNetAddressable();
	LetterMesh->SetIsReplicated(true);
	LetterMesh->SetEnableGravity(false);
	LetterMesh->AlwaysLoadOnClient = true;
	LetterMesh->AlwaysLoadOnServer = true;
	LetterMesh->bReceivesDecals = false;
	LetterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LetterMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
	LetterMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 75.0f));
	LetterMesh->SetMobility(EComponentMobility::Movable);
	LetterMesh->AttachParent = RootComponent;

	if (MeshSpinner != NULL)
	{
		MeshSpinner->RotationRate.Yaw = 160.0f;
		MeshSpinner->UpdatedComponent = LetterMesh;
	}

	MessageClass = UUTDoubleDomGameMessage::StaticClass();
	bReplicates = true;
	SetReplicates(true);
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	ControllingTeamNum = 255;
	NetPriority = 3.0;
	MyObjectiveType = EDomObjectiveType::xPoint;
}

void AxDomPoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AxDomPoint, LetterMesh);
}

void AxDomPoint::UpdateStatus_xPoint()
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

	AUTDoubleDomGameState* GS = Cast<AUTDoubleDomGameState>(GetWorld()->GetGameState<AUTGameState>());
	ControllingTeamNum = newTeam;
	ControllingTeam = Cast<AUTDomTeamInfo>(ControllingPawn->Team);
	ControlledTime = GetWorld()->GetTimeSeconds();
	if (ControllingTeam == NULL || GetControllingTeamNum() >= 5)
	{
		bScoreReady = false;
		if (GS){
			GS->UpdateControlPointFX(this, 5);
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
			AUTDoubleDomGameMode* GM = GetWorld()->GetAuthGameMode<AUTDoubleDomGameMode>();
			if (GM)
			{
				GM->ResetCount();
			}

			if (!GetWorldTimerManager().IsTimerActive(ScoreTimeNotifyHandle))
			{
				bScoreReady = false;
				GetWorldTimerManager().SetTimer(ScoreTimeNotifyHandle, this, &AxDomPoint::SendHomeWithNotify, ScoreTime, false);
			}
		}
	}
}

void AxDomPoint::UpdateTeamEffects_Implementation(int32 TeamIndex)
{
	if (TeamIndex == 5)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		LetterMesh->SetHiddenInGame(true);
		LetterMesh->SetVisibility(false);
		DomLight->SetVisibility(false);
		DomMesh->SetMaterial(0, BaseMeshSkin[5]);
	}
	else
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		LetterMesh->SetHiddenInGame(false);
		LetterMesh->SetVisibility(true);
		DomLight->SetVisibility(true);
		if (LetterMeshSkins.IsValidIndex(TeamIndex))
		{
			LetterMesh->SetMaterial(0, LetterMeshSkins[TeamIndex]);
		}
		if (BaseMeshSkin.IsValidIndex(TeamIndex))
		{
			DomMesh->SetMaterial(0, BaseMeshSkin[TeamIndex]);
		}
		if (DomLightColor.IsValidIndex(TeamIndex))
		{
			DomLight->SetLightColor(DomLightColor[TeamIndex]);
		}
	}
	ForceNetUpdate();
}
