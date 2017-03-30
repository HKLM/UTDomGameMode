/**
 * Control Point Actor - Base Game Objective of the Domination Game Modes.
 * This actor will spawn a TSubclassOf<AControlPoint> that will be used by the
 * current game mode.
 * Created by Brian 'Snake' Alexander, 2015
 */
#pragma once

#include "UnrealTournament.h"
#include "MultiTeamTeamInfo.h"
#include "UTADomTypes.h"
#include "UTMultiTeamTypes.h"
#include "Net/UnrealNetwork.h"
#include "CollisionQueryParams.h"
#include "UTResetInterface.h"
#include "ControlPoint.generated.h"

class AUTDomControlPoint;
class AxDomPointA;
class AxDomPointB;

extern FCollisionResponseParams WorldResponseParams;

/** Control Point is the objective actor for the Domination game modes. */
UCLASS(HideCategories = GameObject, autoexpandcategories = ControlPoint, meta=(ShortTooltip="Control Point is the objective actor for the Domination game modes."))
class UTDOMGAMEMODE_API AControlPoint : public AUTGameObjective, public IUTResetInterface
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	UPROPERTY(Replicated, EditInstanceOnly, Category = ControlPoint)
	TEnumAsByte<EControlPoint::Type> ObjectiveType;

	UPROPERTY(Replicated, BlueprintReadOnly)
	AControlPoint* ThisControlPoint;

	/** The name to display on HUD of this point */
	UPROPERTY(Replicated, EditInstanceOnly, BlueprintReadWrite, Category = ControlPoint)
	FString PointName;

	/** The controlling pawn. Replicated */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = ControlPoint)
	AUTPlayerState* ControllingPawn;

	/** The controlling team.  replicated */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = ControlPoint)
	AMultiTeamTeamInfo* ControllingTeam;

	/** Sound to play when point is captured */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* ControlPointCaptureSound;

	TSubclassOf<UUTLocalMessage> MessageClass;

	//=========================================================================
	// Components 

	USceneComponent* SceneRoot;
	UPROPERTY(EditInstanceOnly)
	UCapsuleComponent* DomCollision;

	/** The point light that displays the controlling teams color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ControlPoint)
	class UPointLightComponent* DomLight;

	/** Array of light colors for DomLight. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ControlPoint)
	TArray<FLinearColor> DomLightColor;

	/** Array of staticmeshes to display on DomMesh. Index = TeamNum */
	UPROPERTY()
	TArray<UStaticMesh*> TeamMesh;

	UPROPERTY(Transient)
	bool bStopControlledTimer;

	FTimerHandle ControlledTimerHandle;

	//=========================================================================

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UBillboardComponent* EditorSprite;
#endif

protected:
#if WITH_EDITORONLY_DATA

	UPROPERTY(Transient)
	class UStaticMeshComponent* EditorDomMesh;
	UPROPERTY(Transient)
	class URotatingMovementComponent* EditorMeshSpinner;
#endif
	/** This should only be true for subclass actors spawned by AControlPoint */
	UPROPERTY(Replicated)
	bool bIsGameObjective;

	float ControlledTime;

	/** will be 'true' if and when the domination point can be captured */
	UPROPERTY(Transient)
	bool bScoreReady;

	/**
	 * The ammount of time other teams must wait after, the current team has touched the control point,
	 * before it will allow the next team to be able to touch it.
 	 */
	UPROPERTY(Transient)
	float ScoreTime;

	/** timer for stats- how long this has been controlled by a team */
	virtual void TeamHeldTimer();

	/** Timer that counts down the ScoreTime and then calls SendHomeWithNotify() */
	FTimerHandle ScoreTimeNotifyHandle;
	UFUNCTION()
	virtual void ScoreTimeNotify();

public:
	virtual void Init(AControlPoint* RegThis) {};

	/**
	 * This should only return true for subclass actors spawned by AControlPoint. 
	 * AControlPoint should allways return False;
	 */
	bool inline GetIsGameObjective() const
	{
		return bIsGameObjective;
	};

	UFUNCTION()
	bool GetIsScoreReady() const { return bScoreReady; };

	/** Updates players stats of how many times this has been captured by a player */
	UFUNCTION()
	virtual void UpdateHeldPointStat(AUTPlayerState* thePlayer, float ScoreAmmount);

	/**
	 * Gets point name.
	 * @return	The point name.
	 */
	UFUNCTION(BlueprintPure, Category = ControlPoint)
	virtual FString GetPointName() const;

	/**
	 * Gets the current playerState of the control point holder.
	 * @return	AUTPlayerState	the control point holder otherwise returns NULL.
	 */
	UFUNCTION(BlueprintPure, Category = ControlPoint)
	virtual AUTPlayerState* GetControlPointHolder() const;

	/**
	 * Returns a valid TeamNum for DOM
	 * @return	int8	current status
	 * @note	0=Red Team
	 *			1=Blue Team
	 *			2=Green Team
	 *			3=Gold Team
	 *			4=Neutral/No Team
	 *			5=Disabled (Reserved for DDOM)
	 */
	UFUNCTION(BlueprintPure, Category = ControlPoint)
	virtual uint8 GetTeamNum() const;

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
	virtual void UpdateTeamEffects(uint8 TeamIndex);

	/**
	 * For use in BluePrints for Map Authors to trigger differant code depending on what team, without having to, do a lot of trying to figure out what team triggered it.
	 * @note		EASY SETUP: Drag out from the Return Value to place a new node and select "Utilities|Flow Control|Switch|Switch on EControllingTeam"
	 *				Now connect the Exec of the switch to OnActorBeginOverlap of the ControlPoint. Now you have a easy way to trigger team based stuff
	 *
	 * @return		EControllingTeamEnum of the ControllingTeam
	 */
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
	TEnumAsByte<EControllingMultiTeam::Type> NotifyTeamChanged() const;

	/**
	 * Reset or reset and disable this control point. Clear controlling team, pawn, etc
	 * @param	IsEnabled	True=Reset this control point, False=Disable this control point.
	 */
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
	virtual void ResetPoint(bool IsEnabled);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Game)
	void Reset() override;

	/** Disables the point and removes it from play */
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
	void DisablePoint();
	virtual void Disable_Implementation();

	/*** Override the following ***/
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual AUTPlayerState* GetCarriedObjectHolder() override;
	// not applicable
	virtual void ObjectStateWasChanged(FName NewObjectState) override {};
	virtual void ObjectWasPickedUp(AUTCharacter* NewHolder, bool bWasHome) override {};
	virtual void ObjectWasDropped(AUTCharacter* LastHolder) override {};
	virtual void ObjectReturnedHome(AUTCharacter* Returner) override {};
	virtual void SetTeamForSideSwap_Implementation(uint8 NewTeamNum) override {};

protected:
	virtual void CreateCarriedObject() override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent)
	void ProcessTouch(APawn* TouchedBy);
};
