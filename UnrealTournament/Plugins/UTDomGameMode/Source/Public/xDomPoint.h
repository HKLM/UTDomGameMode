// Created by Brian 'Snake' Alexander, 2017
#pragma once

#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "UTADomTypes.h"
#include "xDomPoint.generated.h"

UCLASS(Abstract, Blueprintable, NotPlaceable)
class UTDOMGAMEMODE_API AxDomPoint : public AControlPoint
{
	GENERATED_UCLASS_BODY()

	// Skins for the floor BaseMesh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = XDomPoint)
	TArray<UMaterialInstanceConstant*> BaseMeshSkin;
		
	// Skins for the Letter (DomMesh)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = XDomPoint)
	TArray<UMaterialInstanceConstant*> LetterSkins;

	// Skins for the ring mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = XDomPoint)
	TArray<UMaterialInstanceConstant*> RingSkins;

	// floor base mesh
	UPROPERTY(Replicated, VisibleAnywhere)
	class UStaticMeshComponent* BaseMesh;

	UPROPERTY(Replicated, VisibleAnywhere)
	class UStaticMeshComponent* LetterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = XDomPoint)
	class URotatingMovementComponent* LetterSpinner;

	// The big Ring staticmesh
	UPROPERTY(Replicated, VisibleAnywhere)
	class UStaticMeshComponent* RingMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = XDomPoint)
	class URotatingMovementComponent* RingSpinner;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void UpdateStatus() override;
	virtual void UpdateTeamEffects_Implementation(uint8 TeamIndex) override;
	virtual void Init(AControlPoint* RegThis) override;

protected:
	virtual void CreateCarriedObject() override {};
};