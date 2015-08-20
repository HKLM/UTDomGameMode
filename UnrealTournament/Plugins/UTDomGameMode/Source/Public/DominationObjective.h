/**
* This is the actor map authors place with in the map
* that will spawn the correct type of ControlPoint
* depending on what game mode it is.
* Created by Brian 'Snake' Alexander, 2015
**/
#pragma once

#include "UnrealTournament.h"
#include "CollisionQueryParams.h"
#include "ControlPoint.h"
#include "DominationObjective.generated.h"

extern FCollisionResponseParams WorldResponseParams;

UCLASS(Blueprintable, HideCategories = GameObject)
class ADominationObjective : public AUTGameObjective
{
	GENERATED_UCLASS_BODY()

	/** This points name to display on the HUD **/
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = ControlPoint)
		FString PointName;

	/** The ControlPoint actor that is spawned at game time */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = ControlPoint)
		AControlPoint* MyControlPoint;

	/** The sound to play when this point is captured */
	UPROPERTY(BlueprintReadWrite, Category = ControlPoint)
		USoundBase* ControlPointCaptureSound;

	/**
	* Internaly set by the UTDomGameMode->DominationObjectiveType
	* Class of the control point to be spawned. 
	*/
	UPROPERTY(Replicated, BlueprintReadOnly, Category = ControlPoint)
		TSubclassOf<class ABaseControlPoint> MyDomObjectiveType;

	/** point light component. Only for show in Editor, is not used in play. */
	UPROPERTY()
	class UPointLightComponent* DomLight;

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

	virtual uint8 GetTeamNum() const override
	{
		return (MyControlPoint != NULL) ? MyControlPoint->ControllingTeamNum : 255;
	}

	/**
	 * Gets point name.
	 * @return	The point name.
	 */
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual FString GetPointName();

	/**
	 * Gets control point spawned at this point.
	 * @return	AControlPoint	the control point.
	 */
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual AControlPoint* GetControlPoint();

	virtual void InitializeObjective() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ObjectStateWasChanged(FName NewObjectState) override {};
	virtual void ObjectWasDropped(AUTCharacter* LastHolder) override {};
	virtual void ObjectWasPickedUp(AUTCharacter* NewHolder, bool bWasHome) override {};
	virtual void ObjectReturnedHome(AUTCharacter* Returner) override {};
	virtual AUTPlayerState* GetCarriedObjectHolder() override;
	virtual FName GetCarriedObjectState() const override;

protected:
	virtual void CreateCarriedObject() override;

	/** StaticMesh displayed only within the Editor */
	UPROPERTY()
	class UStaticMeshComponent* EditorMesh;
};
