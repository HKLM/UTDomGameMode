// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTDomGameMessage.h"
#include "UTDomTeamInfo.h"
#include "Net/UnrealNetwork.h"
#include "CollisionQueryParams.h"
#include "ControlPoint.generated.h"

extern FCollisionResponseParams WorldResponseParams;

UCLASS(HideCategories = GameObject, ShowCategories = ControlPoint)
class AControlPoint : public AUTGameObjective
{
	GENERATED_UCLASS_BODY()

	/** This points name to display on HUD */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Replicated, Category = ControlPoint)
		FString PointName;

	/** The controlling pawn. Replicated */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = ControlPoint)
		AUTPlayerState* ControllingPawn;

	/** The controlling team.  replicated */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = ControlPoint)
		AUTDomTeamInfo* ControllingTeam;

	UPROPERTY(BlueprintReadOnly, Category = ControlPoint)
	class AUTDomGameState* DomGameState;

	USceneComponent* SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = ControlPoint)
		UCapsuleComponent* DomCollision;

	/** Sound to play when point is captured */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
		USoundBase* ControlPointCaptureSound;

	UPROPERTY(Replicated)
		float ControlledTime;

	/** The mesh that makes up this base. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* DomMesh;

	UPROPERTY(VisibleDefaultsOnly)
	class URotatingMovementComponent* MeshSpinner;

	/** The point light that displays the controlling teams color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ControlPoint)
	class UPointLightComponent* DomLight;

	/** Array of light colors for DomLight. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ControlPoint)
		TArray<FLinearColor> DomLightColor;

	/** Array of staticmeshes to display on DomMesh. Index = TeamNum */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ControlPoint)
		TArray<UStaticMesh*> TeamMesh;

	/** will be 'true' if and when the domination point can be captured */
	UPROPERTY()
		bool bScoreReady;

	/** The ammount of time other teams must wait after, the current team has touched the control point, 
	* before it will allow the next team to be able to touch it */
	UPROPERTY(Replicated)
		float ScoreTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ControlPoint)
		TSubclassOf<UUTLocalMessage> MessageClass;

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
	* Gets control point holder.
	* @return	AUTPlayerState	the control point holder otherwise
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

	/*!
	* Returns a valid TeamNum for DOM
	* @return	int8	Number( >= 0 && <= 4 )
	* @note	0=Red Team
	*		1=Blue Team
	*		2=Green Team
	*		3=Gold Team
	*		4=Neutral/No Team
	*		5=Disabled (Reserved for DDOM)
	*/
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual uint8 GetTeamNum() const
	{
		return (TeamNum  == 255) ? 4 : TeamNum;
	}

	virtual void SetTeam(AUTTeamInfo* NewTeam);

	/**
	* Gets point name.
	* @return	The point name.
	*/
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual FString GetPointName();

	/** Updates the status */
	UFUNCTION()
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
