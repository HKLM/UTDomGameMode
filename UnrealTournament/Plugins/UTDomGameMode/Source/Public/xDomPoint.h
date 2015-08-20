// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "xDomPoint.generated.h"

UCLASS(Blueprintable, NotPlaceable)
class AxDomPoint : public AControlPoint
{
	GENERATED_UCLASS_BODY()

	// Skins for the BaseMesh
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = ControlPoint)
		TArray<UMaterialInstanceConstant*> BaseMeshSkin;

	// Skins for the LetterMesh
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = ControlPoint)
		TArray<UMaterialInstanceConstant*> LetterMeshSkins;

	// The big letter staticmesh
	UPROPERTY(Replicated)
	class UStaticMeshComponent* LetterMesh;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void UpdateStatus_xPoint() override;
	void UpdateTeamEffects_Implementation(int32 TeamIndex) override;
};