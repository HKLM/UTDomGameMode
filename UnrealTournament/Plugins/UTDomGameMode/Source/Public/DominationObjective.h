#pragma once

#include "UnrealTournament.h"
#include "UTGameObjective.h"
#include "DominationObjective.generated.h"


UCLASS(deprecated)
class ADEPRECATED_DominationObjective : public AUTGameObjective
{
	GENERATED_UCLASS_BODY()

	/** This points name to display on the HUD **/
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = ControlPoint)
		FString PointName;

	/** The sound to play when this point is captured */
	UPROPERTY(BlueprintReadWrite, Category = ControlPoint)
		USoundBase* ControlPointCaptureSound;

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
		return 255;
	}

	/**
	 * Gets point name.
	 * @return	The point name.
	 */
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual FString GetPointName();

	virtual void InitializeObjective() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ObjectStateWasChanged(FName NewObjectState) override {};
	virtual void ObjectWasPickedUp(AUTCharacter* NewHolder, bool bWasHome) override {};
	virtual void ObjectWasDropped(AUTCharacter* LastHolder) override {};
	virtual void ObjectReturnedHome(AUTCharacter* Returner) override {};
	virtual AUTPlayerState* GetCarriedObjectHolder() override;
	virtual void SetTeamForSideSwap_Implementation(uint8 NewTeamNum) override {};
	
protected:

	virtual void CreateCarriedObject() override;
	virtual void OnObjectStateChanged() override {};
	/** StaticMesh displayed only within the Editor */
	UPROPERTY()
	class UStaticMeshComponent* EditorMesh;
};
