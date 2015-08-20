// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "BaseControlPoint.h"
#include "Net/UnrealNetwork.h"
#include "CollisionQueryParams.h"
#include "UTADomTypes.h"
#include "ControlPoint.generated.h"

extern FCollisionResponseParams WorldResponseParams;

UCLASS(Blueprintable, NotPlaceable)
class AControlPoint : public ABaseControlPoint
{
	GENERATED_UCLASS_BODY()

	/** Sound to play when point is captured */
	UPROPERTY(BlueprintReadWrite, Category = Sound)
		USoundBase* ControlPointCaptureSound;

	UPROPERTY(Replicated)
		float ControlledTime;

	/** The mesh that makes up this base. */
	UPROPERTY(Replicated)
	class UStaticMeshComponent* DomMesh;

	UPROPERTY()
	class URotatingMovementComponent* MeshSpinner;

	/** The point light that displays the controlling teams color */
	UPROPERTY(BlueprintReadWrite, Category = ControlPoint)
	class UPointLightComponent* DomLight;

	/** The dom light color. */
	UPROPERTY(BlueprintReadWrite, Category = ControlPoint)
		TArray<FLinearColor> DomLightColor;

	UPROPERTY(BlueprintReadOnly, Category = ControlPoint)
		TArray<UStaticMesh*> TeamMesh;

	/** Type of my objective. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = DOM)
		EDomObjectiveType MyObjectiveType;

	/** will be 'true' if and when the domination point can be captured */
	UPROPERTY()
		bool bScoreReady;

	/** The time after one team touches a control point, til the next team can capture it */
	UPROPERTY(Replicated)
		float ScoreTime;

	/** Timer that counts down the ScoreTime and then calls SendHomeWithNotify() */
	FTimerHandle ScoreTimeNotifyHandle;
	virtual void SendHomeWithNotify() override;

	/**	@returns the Control Point */
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual AControlPoint* GetControlPoint();

	/**
	* Reset or reset and disable this control point. Clear controlling team, pawn, etc
	* @param	IsEnabled	True=Reset this control point, False=Disable this control point.
	*/
	virtual void ResetPoint(bool IsEnabled);
	virtual void Reset_Implementation();

	/** Updates the status */
	virtual void UpdateStatus() override;

	virtual void UpdateStatus_ControlPoint();
	virtual void UpdateStatus_xPoint();

	UFUNCTION()
		virtual void OnOverlapEnd_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent)
		void ProcessTouch(APawn* TouchedBy);

	/**
	* Updates effects and send out messages when the status of this point has changed.
	* @note this should not be called directly. Use AUTDomGameState->UpdateControlPointFX()
	* 		for changes to be replicated to clients.
	*		ControllingPawn should be set before calling this and child classes need to
	*		set ControllingTeam before calling super.UpdateStatus()
	* @param	TeamIndex	The teamID of what team to change the mesh and materials to
	*/
	UFUNCTION(NetMulticast, Reliable)
		void UpdateTeamEffects(int32 TeamIndex);

	/**
	* Gets team event name.
	* @param	TeamID	Identifier for the team.
	* @return	The team event name.
	*/
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual FName GetTeamEventName(int32 TeamID) const;

	void Init(AUTGameObjective* NewBase);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Disables the point and removes it from play */
	void DisablePoint();

	UFUNCTION()
		virtual void Disable_Implementation();

	/*** Override the following ***/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetHolder(AUTCharacter* NewHolder) override;
	void ChangeState(FName NewCarriedObjectState);
	virtual void AttachTo(USkeletalMeshComponent* AttachToMesh) override {};
	virtual void DetachFrom(USkeletalMeshComponent* AttachToMesh) override {};
	virtual void SendHome() override {};
	void Score_Implementation(FName Reason, AUTCharacter* ScoringPawn, AUTPlayerState* ScoringPS) override {};
	void TryPickup_Implementation(AUTCharacter* Character) override {};
	virtual void OnRep_AttachmentReplication() override {};
	virtual void OnRep_ReplicatedMovement() override {};
	virtual void GatherCurrentMovement() override {};
	virtual float GetHeldTime(AUTPlayerState* TestHolder) override
	{
		return 0.0f;
	};
	virtual void OnObjectStateChanged() override;
	virtual void FellOutOfWorld(const UDamageType& dmgType) override {};

protected:
	virtual void OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult) override;

	virtual void MoveToHome() override {};
	virtual void TossObject(AUTCharacter* ObjectHolder) override {};
	virtual void OnHolderChanged() override;
	virtual void PickupDenied(AUTCharacter* Character) override {};
	virtual void NoLongerHeld(AController* InstigatedBy = NULL) override {};
	virtual bool CanBePickedUpBy(AUTCharacter* Character) override
	{
		return false;
	};
	virtual void SendGameMessage(uint32 Switch, APlayerState* PS1, APlayerState* PS2, UObject* OptionalObject = NULL) {};
};
