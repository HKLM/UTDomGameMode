// Created by Brian 'Snake' Alexander, 2015
#pragma once

#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "UTHUDWidget_DOMStatus.generated.h"

USTRUCT()
struct FPointInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DOM.HUD")
		FVector2D StatusIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DOM.HUD")
		AControlPoint* thePoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DOM.HUD")
		FHUDRenderObject_Texture DomArrowDir;
};

UCLASS(Config = UTDomGameMode)
class UUTHUDWidget_DOMStatus : public UUTHUDWidget
{
	GENERATED_UCLASS_BODY()

	/** colors assigned to the teams */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DOM.HUD")
		TArray<FLinearColor> TeamColors;

	/* The control points */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DOM.HUD")
		TArray<FPointInfo> CtrlPoints;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DOM.HUD")
		FVector2D DomPosition;

	/* array of dom team icon HUD texture */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DOM.HUD")
		TArray<UTexture2D*> DomTeamIconTexture; 

	/** draw direction arrow for each control point on HUD */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "DOM.HUD")
		bool bDrawDirectionArrow;   

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DOM.HUD")
		UTexture2D* ArrowDirTexture;

	UPROPERTY(Transient)
		bool bControlPointInitialized;

	/** Base size of HUD icon for each control point **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DOM.HUD")
		float IcoMulti;

	UPROPERTY()
		float IconSize;

	// used as a quick way not to have to recalculate the icons. 
	// Copy of RenderScale to compair this time against. If differant then we will have to resetup the icons
	UPROPERTY()
		float LastRenderScale;

	virtual void Draw_Implementation(float DeltaTime);
	virtual void InitializeWidget(AUTHUD* Hud);

	virtual bool ShouldDraw_Implementation(bool bShowScores)
	{
		return !bShowScores;
	}

	virtual void FindControlPoints();

	UFUNCTION(BlueprintCallable, Category = "DOM.HUD")
		virtual UTexture2D* GetDomTeamIcon(uint8 TeamIndex);
};