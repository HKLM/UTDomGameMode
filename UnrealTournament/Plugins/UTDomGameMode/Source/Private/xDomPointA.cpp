// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTADomTypes.h"
#include "xDomPoint.h"
#include "xDomPointA.h"

AxDomPointA::AxDomPointA(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ObjectiveType = EControlPoint::PT_xDomA;
	PointName = "A";

	LetterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LetterMesh->SetStaticMesh(TeamMesh[0]);
	LetterMesh->AlwaysLoadOnClient = true;
	LetterMesh->AlwaysLoadOnServer = true;
	LetterMesh->bCastDynamicShadow = false;
	LetterMesh->bAffectDynamicIndirectLighting = true;
	LetterMesh->bReceivesDecals = false;
	LetterMesh->SetWorldScale3D(FVector(0.3f));
	LetterMesh->RelativeLocation.Z = 238;
	LetterMesh->SetMaterial(0, LetterSkins[4]);
}
