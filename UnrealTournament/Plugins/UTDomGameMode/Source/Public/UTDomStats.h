// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "UTDomStats.generated.h"

//static const FName NAME_DomPointHeldTime(TEXT("DomPointHeldTime"));
//static const FName NAME_TeamDomPointHeldTime(TEXT("TeamDomPointHeldTime"));
//static const FName NAME_DomPointCaptures(TEXT("DomPointCaptures"));
//static const FName NAME_TeamDomPointCaptures(TEXT("TeamDomPointCaptures"));

UCLASS()
class AUTDomStats : public AInfo
{
	GENERATED_UCLASS_BODY()

		/** Cached reference to the DOM game state */
		//UPROPERTY(BlueprintReadOnly, Category = DOM)
		//	class AUTDomGameState* DomGameState;

		//UPROPERTY()
		//	float FlagKillHeldBonus;

		//UPROPERTY()
		//	float FlagKillHeldBonus;

		//virtual void BeginPlay() override;
		//virtual void FlagHeldTimer();
		//virtual void ScoreTeam(uint8 ControlPointIndex, float TeamScoreAmount);
		//virtual void ScoreDamage(int32 DamageAmount, AController* Victim, AController* Attacker);
		//virtual void ScoreKill(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType);
		//virtual void ScoreObject(AUTCarriedObject* GameObject, AUTCharacter* HolderPawn, AUTPlayerState* Holder, FName Reason, float TimeLimit);

		//virtual bool WasThreateningFlagCarrier(AUTPlayerState *VictimPS, APawn* KilledPawn, AUTPlayerState *KillerPS);

		/** Return how long flag was held before current scoring action. */
		//virtual float GetTotalHeldTime(AUTCarriedObject* GameObject);

};
