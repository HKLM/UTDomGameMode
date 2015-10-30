// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTLocalMessage.h"
#include "UTDomTeamInfo.h"
#include "Net/UnrealNetwork.h"
#include "CollisionQueryParams.h"
#include "ControlPoint.generated.h"

extern FCollisionResponseParams WorldResponseParams;

UCLASS(HideCategories = GameObject)
class AControlPoint : public AUTGameObjective
{
	GENERATED_UCLASS_BODY()

	/** This points name to display on HUD */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Replicated, Category = ControlPoint)
		FString PointName;

	/** The controlling pawn. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = ControlPoint)
		AUTPlayerState* ControllingPawn;

	/** The controlling team. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = ControlPoint)
		AUTDomTeamInfo* ControllingTeam;

	/** The controlling team number. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = ControlPoint)
		int32 ControllingTeamNum;

	USceneComponent* SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = ControlPoint)
		UCapsuleComponent* DomCollision;

	/** Sound to play when point is captured */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
		USoundBase* ControlPointCaptureSound;

	UPROPERTY(Replicated)
		float ControlledTime;

	/** The mesh that makes up this base. */
	UPROPERTY(Replicated, EditAnywhere)
	class UStaticMeshComponent* DomMesh;

	UPROPERTY(VisibleDefaultsOnly)
	class URotatingMovementComponent* MeshSpinner;

	/** The point light that displays the controlling teams color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ControlPoint)
	class UPointLightComponent* DomLight;

	/** The dom light color. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ControlPoint)
		TArray<FLinearColor> DomLightColor;

	/** the staticmesh to display. Index = TeamNum */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ControlPoint)
		TArray<UStaticMesh*> TeamMesh;

	/** will be 'true' if and when the domination point can be captured */
	UPROPERTY()
		bool bScoreReady;

	/** The time after one team touches a control point, til the next team can capture it */
	UPROPERTY(Replicated)
		float ScoreTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ControlPoint)
		TSubclassOf<UUTLocalMessage> MessageClass;

	/** Timer that counts down the ScoreTime and then calls SendHomeWithNotify() */
	FTimerHandle ScoreTimeNotifyHandle;
	virtual void ScoreTimeNotify();

	/**
	* Gets controlling team number.
	* @return	int32	The controlling team number.
	*/
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual int32 GetControllingTeamNum();

	/**
	* Gets control point holder.
	* @return	AUTPlayerState	the control point holder.
	* ### summary	@returns the PlayerState of the UTCharacter holding CarriedObject otherwise
	* 				returns NULL.
	*/
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual AUTPlayerState* GetControlPointHolder();

	/**
	* Reset or reset and disable this control point. Clear controlling team, pawn, etc
	* @param	IsEnabled	True=Reset this control point, False=Disable this control point.
	*/
	virtual void ResetPoint(bool IsEnabled);
	virtual void Reset_Implementation();

	// not applicable
	virtual void SetTeamForSideSwap_Implementation(uint8 NewTeamNum) override
	{}

	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual uint8 GetTeamNum() const
	{
		return (ControllingTeamNum != 255) ? ControllingTeamNum : 255;
	}

	virtual void SetTeam(AUTTeamInfo* NewTeam);

	/**
	* Gets point name.
	* @return	The point name.
	*/
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual FString GetPointName();

	/** Updates the status */
		virtual void UpdateStatus();

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

	/** Disables the point and removes it from play */
	void DisablePoint();

	UFUNCTION()
		virtual void Disable_Implementation();

	/*** Override the following ***/
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ObjectStateWasChanged(FName NewObjectState) override {};
	virtual void ObjectWasPickedUp(AUTCharacter* NewHolder, bool bWasHome) override {};
	virtual void ObjectWasDropped(AUTCharacter* LastHolder) override {};
	virtual void ObjectReturnedHome(AUTCharacter* Returner) override {};
	virtual AUTPlayerState* GetCarriedObjectHolder() override;

protected:
	virtual void CreateCarriedObject() {};

	UFUNCTION()
		virtual void OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	UFUNCTION()
		virtual void OnOverlapEnd_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent)
		void ProcessTouch(APawn* TouchedBy);
};
