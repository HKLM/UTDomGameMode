// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTDomTeamInfo.h"
#include "Net/UnrealNetwork.h"
#include "UTPathBuilderInterface.h"
#include "BaseControlPoint.generated.h"

UCLASS(Blueprintable, Abstract)
class ABaseControlPoint : public AUTCarriedObject, public IUTPathBuilderInterface
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

	/**
	* Gets the ControllingTeamNum value
	* @return	int32	int of the controlling TeamNum (0-3), (5=Disabled) or (4,6-255=Neutral)
	**/
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		int32 GetControllingTeamNum();

	/**	@returns the PlayerState of the UTCharacter holding CarriedObject otherwise returns NULL */
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual AUTPlayerState* GetControlPointHolder();

	virtual uint8 GetTeamNum() const override
	{
		return (ControllingTeamNum != 255) ? ControllingTeamNum : 255;
	}

	/**
	* Gets point name.
	* @return	The point name.
	*/
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual FString GetPointName();

	/** Updates the status */
	UFUNCTION(BlueprintCallable, Category = ControlPoint)
		virtual void UpdateStatus() {};

	/*** Override the following ***/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Use() override {};
	virtual void Drop(AController* Killer = NULL) override {};
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
	virtual void FellOutOfWorld(const UDamageType& dmgType) override {};

protected:
	virtual void MoveToHome() override {};
	virtual void TossObject(AUTCharacter* ObjectHolder) override {};
	virtual void PickupDenied(AUTCharacter* Character) override {};
	virtual void NoLongerHeld(AController* InstigatedBy = NULL) override {};
	virtual bool CanBePickedUpBy(AUTCharacter* Character) override
	{
		return false;
	};
	virtual void SendGameMessage(uint32 Switch, APlayerState* PS1, APlayerState* PS2, UObject* OptionalObject = NULL) {};
};
