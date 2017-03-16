/**
 * Control Point Actor - Game Objective of Domination Game Mode
 * Created by Brian 'Snake' Alexander, 2017
 */
#pragma once

#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "UTDomControlPoint.generated.h"

/** Control Point is the objective actor for Domination game mode. */
UCLASS(NotPlaceable)
class UTDOMGAMEMODE_API AUTDomControlPoint : public AControlPoint
{
	GENERATED_UCLASS_BODY()

	/** The mesh that is displayed, that makes up this base. */
	UPROPERTY(Replicated)
	class UStaticMeshComponent* DomMesh;

	/** Mesh Spinner */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ControlPoint)
	class URotatingMovementComponent* MeshSpinner;

	virtual void Init(AControlPoint* RegThis) override;

	/**
	 * Updates effects and send out messages when the status of this point has changed.
	 * @note this should not be called directly. Use AUTDomGameState->UpdateControlPointFX()
	 * 		for changes to be replicated to clients.
	 *		ControllingPawn should be set before calling this and child classes need to
	 *		set ControllingTeam before calling super.UpdateStatus()
	 * @param	TeamIndex	The teamID of what team to change the mesh and materials to
	 */
	virtual void UpdateTeamEffects_Implementation(uint8 TeamIndex) override;

	virtual void Disable_Implementation() override;

	/*** Override the following ***/
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void CreateCarriedObject() override {};
};
