// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTADomTypes.h"
#include "xDomPoint.h"
#include "xDomPointB.h"

AxDomPointB::AxDomPointB(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ObjectiveType = EControlPoint::PT_xDomB;
	PointName = "B";

	LetterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LetterMesh->SetStaticMesh(TeamMesh[1]);
	LetterMesh->AlwaysLoadOnClient = true;
	LetterMesh->AlwaysLoadOnServer = true;
	LetterMesh->bCastDynamicShadow = false;
	LetterMesh->bAffectDynamicIndirectLighting = true;
	LetterMesh->bReceivesDecals = false;
	LetterMesh->SetWorldScale3D(FVector(0.3f));
	LetterMesh->RelativeLocation.Z = 238;
	LetterMesh->SetMaterial(0, LetterSkins[4]);
}
