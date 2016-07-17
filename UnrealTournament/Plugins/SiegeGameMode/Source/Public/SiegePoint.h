/**
* Siege Point Actor - Game Objective of Siege Game Mode
* Created by Brian 'Snake' Alexander, (c) 2016
*/
#pragma once

#include "UnrealTournament.h"
#include "SiegeGameMessage.h"
#include "UTDomTeamInfo.h"
#include "UTADomTypes.h"
#include "Net/UnrealNetwork.h"
#include "CollisionQueryParams.h"
#include "SiegePoint.generated.h"

extern FCollisionResponseParams WorldResponseParams;

/** Siege Point is the objective actor for the Siege game mode. */
UCLASS(NotPlaceable)
class ASiegePoint : public AUTCarriedObject
{
	GENERATED_UCLASS_BODY()

	/** The name to display on HUD of this point */
		UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = SiegePoint)
		FString PointName;

	/** The controlling pawn. Replicated */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = SiegePoint)
		AUTPlayerState* ControllingPawn;

	/** The controlling team.  replicated */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = SiegePoint)
		AUTDomTeamInfo* ControllingTeam;

	/** The team this actor belongs to. What team's base it is located in */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = SiegePoint)
		EBaseTeam BaseTeam;

	float ControlledTime;

	UPROPERTY(Transient)
		bool bStopControlledTimer;

	/** will be 'true' if and when the domination point can be captured */
	bool bScoreReady;

	/**
	* The ammount of time other teams must wait after, the current team has touched the control point,
	* before it will allow the next team to be able to touch it.
	*/
	float ScoreTime;

	/** Sound to play when point is captured */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
		USoundBase* SiegePointCaptureSound;

	TSubclassOf<UUTLocalMessage> MessageClass;

	//=========================================================================
	// Components 

	/** The mesh that is displayed, that makes up this base. */
	UPROPERTY(Replicated)
		class UStaticMeshComponent* DomMesh;

	class URotatingMovementComponent* MeshSpinner;

	/** The point light that displays the controlling teams color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SiegePoint)
		class UPointLightComponent* DomLight;

		/** Array of light colors for DomLight. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SiegePoint)
		TArray<FLinearColor> DomLightColor;

	/** Array of staticmeshes to display on DomMesh. Index = TeamNum */
	UPROPERTY()
	TArray<UStaticMesh*> TeamMesh;
	UPROPERTY()
	TArray<UMaterialInstanceConstant*> TeamSkins;

	//=========================================================================

	virtual void Init(AUTGameObjective* NewBase) override;
	virtual void SendHome() override {};
	virtual void SendHomeWithNotify() override {};
	virtual void SetTeam(AUTTeamInfo* NewTeam) {};
	virtual void AttachTo(USkeletalMeshComponent* AttachToMesh) override {};
	virtual void DetachFrom(USkeletalMeshComponent* AttachToMesh) override {};
	virtual void ClientUpdateAttachment(bool bNowAttached) override {};
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Destroyed() override {};
	virtual void OnRep_AttachmentReplication() override {};
	virtual void OnRep_ReplicatedMovement() override {};
	virtual void GatherCurrentMovement() override {};

	virtual float GetHeldTime(AUTPlayerState* TestHolder) override { return 0.0f; };
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override
	{
		Super::AActor::PreReplication(ChangedPropertyTracker);
	}
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override {};
	virtual void CheckTouching() override {};
protected:
	virtual void OnObjectStateChanged() {};

	virtual void OnHolderChanged() override {};

	virtual void OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual bool CanBePickedUpBy(AUTCharacter* Character) override { return false; };
	virtual void NoLongerHeld(AController* InstigatedBy = NULL) override {};
	virtual void MoveToHome() override {};
	virtual void SendGameMessage(uint32 Switch, APlayerState* PS1, APlayerState* PS2, UObject* OptionalObject = NULL) override {};
	virtual bool TeleportTo(const FVector& DestLocation, const FRotator& DestRotation, bool bIsATest = false, bool bNoCheck = false) override { return false; };

public:
	virtual FText GetHUDStatusMessage(AUTHUD* HUD) override { return FText::FromString(" "); };


	/**
	* Gets point name.
	* @return	The point name.
	*/
	UFUNCTION(BlueprintPure, Category = SiegePoint)
	virtual FString GetPointName() const;

	/** timer for stats- how long this has been controlled by a team */
	virtual void TeamHeldTimer();

	/** Updates players stats of how many times this has been captured by a player */
	UFUNCTION()
	virtual void UpdateHeldPointStat(AUTPlayerState* thePlayer, float ScoreAmmount);

	/** Timer that counts down the ScoreTime and then calls SendHomeWithNotify() */
	FTimerHandle ScoreTimeNotifyHandle;
	UFUNCTION()
	virtual void ScoreTimeNotify();

	/**
	* Gets the current playerState of the control point holder.
	* @return	AUTPlayerState	the control point holder otherwise returns NULL.
	*/
	UFUNCTION(BlueprintPure, Category = SiegePoint)
	virtual AUTPlayerState* GetSiegePointHolder() const;

	/*!
	* Returns the TeamNum of the team that currently CONTROLS this point.
	* This is not the team that owns this point, use HomeBase->GetTeamNum() to get owning team
	*/
	UFUNCTION(BlueprintPure, Category = SiegePoint)
	virtual uint8 GetTeamNum() const;

	/** Updates the status */
	UFUNCTION()
	virtual void UpdateStatus();

	/**
	* Updates effects and send out messages when the status of this point has changed.
	* @note this should not be called directly. Use ASiegeGameState->UpdateSiegePointFX()
	* 		for changes to be replicated to clients.
	*		ControllingPawn should be set before calling this and child classes need to
	*		set ControllingTeam before calling super.UpdateStatus()
	* @param	TeamIndex	The teamID of what team to change the mesh and materials to
	*/
	UFUNCTION(NetMulticast, Reliable)
	void UpdateTeamEffects(uint8 TeamIndex);

	/**
	* For use in BluePrints for Map Authors to trigger differant code depending on what team, without having to, do a lot of trying to figure out what team triggered it.
	* @note		EASY SETUP: Drag out from the Return Value to place a new node and select "Utilities|Flow Control|Switch|Switch on EControllingTeam"
	*			Now connect the Exec of the switch to OnActorBeginOverlap of the SiegePoint. Now you have a easy way to trigger team based stuff
	*
	* @return	EControllingTeamEnum of the ControllingTeam
	*/
	UFUNCTION(BlueprintCallable, Category = SiegePoint)
		TEnumAsByte<EControllingTeam::Type> NotifyTeamChanged() const;

	/**
	* Reset or reset and disable this control point. Clear controlling team, pawn, etc
	* @param	IsEnabled	True=Reset this control point, False=Disable this control point.
	*/
	UFUNCTION(BlueprintCallable, Category = SiegePoint)
	virtual void ResetPoint(bool IsEnabled);
	virtual void Reset_Implementation();

	/** Disables the point and removes it from play */
	UFUNCTION(BlueprintCallable, Category = SiegePoint)
	void DisablePoint();
	virtual void Disable_Implementation();

	/*** Override the following ***/
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:

	UFUNCTION()
		virtual void OnOverlapEnd_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent)
		void ProcessTouch(APawn* TouchedBy);
};
