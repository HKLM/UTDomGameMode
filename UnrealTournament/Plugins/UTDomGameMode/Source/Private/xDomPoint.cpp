// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "Net/UnrealNetwork.h"
#include "CollisionQueryParams.h"
#include "ControlPoint.h"
#include "UTDoubleDomGameMessage.h"
#include "UTDoubleDomGameMode.h"
#include "xDomPoint.h"

AxDomPoint::AxDomPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Load - Staticmeshes
	static ConstructorHelpers::FObjectFinder<UStaticMesh> AMesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/xDomA.xDomA'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BMesh(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/xDomB.xDomB'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRing(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/xDomRing.xDomRing'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFloor(TEXT("StaticMesh'/UTDomGameMode/UTDomGameContent/Meshes/xDomBase.xDomBase'"));
	TeamMesh.Insert(AMesh.Object, 0);
	TeamMesh.Insert(BMesh.Object, 1);

	//Base Mesh - materials
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex0(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DomPoint_MI_0.DomPoint_MI_0'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex1(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DomPoint_MI_1.DomPoint_MI_1'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex2(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DomPoint_MI_2.DomPoint_MI_2'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex3(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DomPoint_MI_3.DomPoint_MI_3'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex4(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DomPoint_MI_4.DomPoint_MI_4'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BTex5(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DomPoint_MI_5.DomPoint_MI_5'"));
	BaseMeshSkin.Insert(BTex0.Object, 0);
	BaseMeshSkin.Insert(BTex1.Object, 1);
	BaseMeshSkin.Insert(BTex2.Object, 2);
	BaseMeshSkin.Insert(BTex3.Object, 3);
	BaseMeshSkin.Insert(BTex4.Object, 4);
	BaseMeshSkin.Insert(BTex5.Object, 5);
	//DomLetter - Skins
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LTex0(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DOMLetter_0.DOMLetter_0'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LTex1(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DOMLetter_1.DOMLetter_1'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LTex2(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DOMLetter_2.DOMLetter_2'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LTex3(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DOMLetter_3.DOMLetter_3'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LTex4(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DOMLetter_4.DOMLetter_4'"));
	LetterSkins.Insert(LTex0.Object, 0);
	LetterSkins.Insert(LTex1.Object, 1);
	LetterSkins.Insert(LTex2.Object, 2);
	LetterSkins.Insert(LTex3.Object, 3);
	LetterSkins.Insert(LTex4.Object, 4);
	//DomRing - Skins
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> RTex0(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DOMRing_0.DOMRing_0'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> RTex1(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DOMRing_1.DOMRing_1'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> RTex2(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DOMRing_2.DOMRing_2'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> RTex3(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DOMRing_3.DOMRing_3'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> RTex4(TEXT("MaterialInstanceConstant'/UTDomGameMode/UTDomGameContent/Textures/DDom/DOMRing_4.DOMRing_4'"));
	RingSkins.Insert(RTex0.Object, 0);
	RingSkins.Insert(RTex1.Object, 1);
	RingSkins.Insert(RTex2.Object, 2);
	RingSkins.Insert(RTex3.Object, 3);
	RingSkins.Insert(RTex4.Object, 4);

	//------Components------
	DomCollision->InitCapsuleSize(160.0f, 180.0f);

	BaseMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FName(TEXT("MeshFloor")));
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseMesh->SetStaticMesh(MeshFloor.Object);
	BaseMesh->AlwaysLoadOnClient = true;
	BaseMesh->AlwaysLoadOnServer = true;
	BaseMesh->bAffectDynamicIndirectLighting = true;
	BaseMesh->bReceivesDecals = false;
	BaseMesh->SetMaterial(0, BaseMeshSkin[4]);
	BaseMesh->SetWorldScale3D(FVector(0.5f));
	BaseMesh->SetupAttachment(RootComponent);
	BaseMesh->RelativeLocation.Z = -15.0f;

	LetterMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FName(TEXT("MeshLetter")));
	LetterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LetterMesh->AlwaysLoadOnClient = true;
	LetterMesh->AlwaysLoadOnServer = true;
	LetterMesh->bCastDynamicShadow = false;
	LetterMesh->bAffectDynamicIndirectLighting = true;
	LetterMesh->bReceivesDecals = false;
	LetterMesh->SetupAttachment(RootComponent);
	LetterMesh->RelativeLocation.Z = 300;
	// Spinner
	LetterSpinner = ObjectInitializer.CreateDefaultSubobject<URotatingMovementComponent>(this, FName(TEXT("LetterSpinner")));
	LetterSpinner->UpdatedComponent = LetterMesh;
	LetterSpinner->RotationRate.Yaw = -120.0f;

	RingMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, FName(TEXT("MeshRing")));
	RingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RingMesh->SetStaticMesh(MeshRing.Object);
	RingMesh->AlwaysLoadOnClient = true;
	RingMesh->AlwaysLoadOnServer = true;
	RingMesh->bCastDynamicShadow = false;
	RingMesh->bAffectDynamicIndirectLighting = true;
	RingMesh->bReceivesDecals = false;
	RingMesh->SetWorldScale3D(FVector(0.3f));
	RingMesh->SetupAttachment(RootComponent);
	RingMesh->RelativeLocation.Z = 250;
	// Spinner
	RingSpinner = ObjectInitializer.CreateDefaultSubobject<URotatingMovementComponent>(this, FName(TEXT("RingSpinner")));
	RingSpinner->UpdatedComponent = RingMesh;
	RingSpinner->RotationRate.RotateVector(FVector(0.0f, 200.0f, 0.0f));
	
	MessageClass = UUTDoubleDomGameMessage::StaticClass();
	bReplicates = true;
	SetReplicates(true);
	bReplicateMovement = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0;
	bIsGameObjective = true;
}

void AxDomPoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AxDomPoint, BaseMesh);
	DOREPLIFETIME(AxDomPoint, LetterMesh);
	DOREPLIFETIME(AxDomPoint, RingMesh);
}

void AxDomPoint::Init(AControlPoint* Parent)
{
	bIsGameObjective = true;
}

void AxDomPoint::UpdateStatus()
{
	Super::UpdateStatus();

	if (ControllingTeam && (TeamNum < 4) && (Role == ROLE_Authority))
	{
		AUTDoubleDomGameMode* GM = GetWorld()->GetAuthGameMode<AUTDoubleDomGameMode>();
		if (GM)
		{
			GM->ResetCount();
		}
	}
}

void AxDomPoint::UpdateTeamEffects_Implementation(uint8 TeamIndex)
{
	if (BaseMeshSkin.IsValidIndex(TeamIndex)) { BaseMesh->SetMaterial(0, BaseMeshSkin[TeamIndex]); }
	if (DomLightColor.IsValidIndex(TeamIndex)) { DomLight->SetLightColor(DomLightColor[TeamIndex]); }

	if (TeamIndex == 5)
	{
		DomCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		LetterMesh->SetHiddenInGame(true);
		LetterMesh->SetVisibility(false);
		RingMesh->SetHiddenInGame(true);
		RingMesh->SetVisibility(false);
		DomLight->SetVisibility(false);
	}
	else
	{
		DomCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		LetterMesh->SetHiddenInGame(false);
		LetterMesh->SetVisibility(true);
		RingMesh->SetHiddenInGame(false);
		RingMesh->SetVisibility(true);
		DomLight->SetVisibility(true);
		if (LetterSkins.IsValidIndex(TeamIndex)) { LetterMesh->SetMaterial(0, LetterSkins[TeamIndex]); }
		if (RingSkins.IsValidIndex(TeamIndex)) { RingMesh->SetMaterial(0, RingSkins[TeamIndex]); }
	}
	Super::UpdateTeamEffects_Implementation(TeamIndex);
}
