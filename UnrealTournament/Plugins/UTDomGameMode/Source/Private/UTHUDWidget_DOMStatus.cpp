// Created by Brian 'Snake' Alexander, 2015
#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "UTDomGameState.h"
#include "UTHUDWidget.h"
#include "UTHUDWidget_DOMStatus.h"

UUTHUDWidget_DOMStatus::UUTHUDWidget_DOMStatus(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ScreenPosition = FVector2D(0.0f, 0.18f);
	Size = FVector2D(0.f, 0.f);
	Origin = FVector2D(0.5f, 0.5f);
	bDrawDirectionArrow = true;

	new(TeamColors)FLinearColor(0.8f, 0.1f, 0.15f, 1.0f);
	new(TeamColors)FLinearColor(0.1f, 0.1f, 0.8f, 1.0f);
	new(TeamColors)FLinearColor(0.0f, 0.8f, 0.0f, 1.0f);
	new(TeamColors)FLinearColor(0.9f, 0.98f, 0.0f, 1.0f);
	new(TeamColors)FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

	new(ArrowDirColor)FLinearColor(0.0f, 0.0f, 1.9f, 1.0f);
	new(ArrowDirColor)FLinearColor(1.9f, 0.05f, 0.05f, 1.0f);
	new(ArrowDirColor)FLinearColor(1.0f, 0.1f, 0.1f, 1.0f);
	new(ArrowDirColor)FLinearColor(1.0f, 0.1f, 0.1f, 1.0f);
	new(ArrowDirColor)FLinearColor(1.9f, 0.0f, 0.0f, 1.0f);

	static ConstructorHelpers::FObjectFinder<UTexture2D> Tex0(TEXT("Texture2D'/UTDomGameMode/UTDomGameContent/Textures/RedTeamSymbol.RedTeamSymbol'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Tex1(TEXT("Texture2D'/UTDomGameMode/UTDomGameContent/Textures/BlueTeamSymbol.BlueTeamSymbol'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Tex2(TEXT("Texture2D'/UTDomGameMode/UTDomGameContent/Textures/GreenTeamSymbol.GreenTeamSymbol'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Tex3(TEXT("Texture2D'/UTDomGameMode/UTDomGameContent/Textures/GoldTeamSymbol.GoldTeamSymbol'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Tex4(TEXT("Texture2D'/UTDomGameMode/UTDomGameContent/Textures/NeutralSymbol.NeutralSymbol'"));
	DomTeamIconTexture.Insert(Tex0.Object, 0);
	DomTeamIconTexture.Insert(Tex1.Object, 1);
	DomTeamIconTexture.Insert(Tex2.Object, 2);
	DomTeamIconTexture.Insert(Tex3.Object, 3);
	DomTeamIconTexture.Insert(Tex4.Object, 4);

	IcoMulti = 58.0f;
}

void UUTHUDWidget_DOMStatus::InitializeWidget(AUTHUD* Hud)
{
	Super::InitializeWidget(Hud);
	LastRenderScale = RenderScale;
}

void UUTHUDWidget_DOMStatus::Draw_Implementation(float DeltaTime)
{
	Super::Draw_Implementation(DeltaTime);

	if (LastRenderScale != RenderScale)
	{
		bControlPointInitialized = false;
	}
	IconSize = FMath::Clamp(IcoMulti * RenderScale, 50.0f, 65.0f);

	AUTDomGameState* GS = Cast<AUTDomGameState>(UTGameState);
	if (GS == NULL) {
		return;
	}
	if (!bControlPointInitialized)
	{
		LastRenderScale = RenderScale;
		FindControlPoints();
		DomPosition.Y = 0.0f;
		bool bDoNext = false;
		// Create icon for each control point
		for (uint8 i = 0; i < CtrlPoints.Num(); i++)
		{
			// offset from last icon
			if (bDoNext)
			{
				DomPosition.Y += IconSize * 1.02f;
			}
			else
			{
				bDoNext = true;
			}

			CtrlPoints[i].StatusIcon.Y = DomPosition.Y;
			CtrlPoints[i].StatusIcon.X = DomPosition.X;

			CtrlPoints[i].DomArrowDir.Atlas = UTHUDOwner->HUDAtlas;
			CtrlPoints[i].DomArrowDir.UVs.U = 180.0f;
			CtrlPoints[i].DomArrowDir.UVs.V = 940.0f;
			CtrlPoints[i].DomArrowDir.UVs.UL = 72.0f;
			CtrlPoints[i].DomArrowDir.UVs.VL = 72.0f;
			CtrlPoints[i].DomArrowDir.Position.X = DomPosition.X;
			CtrlPoints[i].DomArrowDir.Position.Y = DomPosition.Y;
			CtrlPoints[i].DomArrowDir.RenderOffset.X = 0.0f;
			CtrlPoints[i].DomArrowDir.RenderOffset.Y = 0.0f;
			CtrlPoints[i].DomArrowDir.RenderColor = ArrowDirColor[4];
			CtrlPoints[i].DomArrowDir.RenderOpacity = UTHUDOwner->GetHUDWidgetBorderOpacity();
			CtrlPoints[i].DomArrowDir.Size.X = IconSize;
			CtrlPoints[i].DomArrowDir.Size.Y = IconSize;
		}
		bControlPointInitialized = true;
	}
	// The TeamIndex of current ControllingTeam
	uint8 nTeam = 0;
	// Temp offset
	FVector2D POS;

	for (uint8 i = 0; i < CtrlPoints.Num(); i++)
	{
		// Set the team icon to display
		if (CtrlPoints[i].thePoint != NULL
			&& CtrlPoints[i].thePoint->ControllingTeam != NULL
			&& CtrlPoints[i].thePoint->ControllingTeam->GetTeamNum() != 255)
		{
			nTeam = CtrlPoints[i].thePoint->ControllingTeam->GetTeamNum();
		}
		else if (CtrlPoints[i].thePoint != NULL
				 && CtrlPoints[i].thePoint->ControllingPawn != NULL
				 && CtrlPoints[i].thePoint->ControllingPawn->Team !=NULL)
		{
			nTeam = CtrlPoints[i].thePoint->ControllingPawn->Team->TeamIndex;
		}
		else
		{
			nTeam = 4;
		}
		// Draw Team Logo
		POS.X = CtrlPoints[i].StatusIcon.X * RenderScale;
		POS.Y = CtrlPoints[i].StatusIcon.Y;
		float WSO = 1.45f * UTHUDOwner->GetHUDWidgetSlateOpacity();
		DrawTexture(DomTeamIconTexture[nTeam], POS.X, POS.Y, IconSize, IconSize, 2, 2, 255, 255, FMath::Clamp(WSO, 0.0f, 1.0f), TeamColors[nTeam]);

		FString work;
		float px = POS.X;
		float py = POS.Y;
		if (CtrlPoints[i].thePoint != NULL)
		{
			// Draw the points name
			work = CtrlPoints[i].thePoint->PointName;
			float HSO = 1.82f * UTHUDOwner->GetHUDWidgetSlateOpacity();
			DrawText(FText::FromString(work), px + (IconSize / 2) - (IconSize * 0.35f), py + (IconSize / 2)  + 0.01f, UTHUDOwner->TinyFont, FLinearColor::Black, 0.7 * RenderScale, FMath::Clamp(HSO, 0.0f, 1.0f), FLinearColor::White, ETextHorzPos::Left, ETextVertPos::Bottom);
			if (bDrawDirectionArrow
				&& UTCharacterOwner != NULL
				&& UTPlayerOwner->GetPawnOrSpectator())
			{
				APawn* P = UTPlayerOwner->GetPawn();
				FRotator Dir = (CtrlPoints[i].thePoint->GetActorLocation() - UTCharacterOwner->GetActorLocation()).Rotation();
				float Yaw = (Dir.Yaw - P->GetViewRotation().Yaw);
				CtrlPoints[i].DomArrowDir.RotPivot = FVector2D(0.5, 0.5);
				CtrlPoints[i].DomArrowDir.Rotation = Yaw;
				float HBO = 1.2f * UTHUDOwner->GetHUDWidgetBorderOpacity();
				DrawTexture(CtrlPoints[i].DomArrowDir.Atlas
							, POS.X
							, POS.Y
							, IconSize
							, IconSize
							, CtrlPoints[i].DomArrowDir.UVs.U
							, CtrlPoints[i].DomArrowDir.UVs.V
							, CtrlPoints[i].DomArrowDir.UVs.UL
							, CtrlPoints[i].DomArrowDir.UVs.VL
							, FMath::Clamp(HBO, 0.0f, 1.0f)
							, ArrowDirColor[nTeam]
							, CtrlPoints[i].DomArrowDir.RenderOffset
							, CtrlPoints[i].DomArrowDir.Rotation
							, CtrlPoints[i].DomArrowDir.RotPivot);
			}
		}
	}
}

/**
 * Searches for the control points.
 */
void UUTHUDWidget_DOMStatus::FindControlPoints()
{
	AUTDomGameState* GS = GetWorld()->GetGameState<AUTDomGameState>();
	if (GS != NULL)
	{
		CtrlPoints.Empty();
		FPointInfo f;
		TArray<AControlPoint*> CP = GS->AUTDomGameState::GetControlPoints();
		for (AControlPoint* C : CP)
		{
			if (C != NULL)
			{
				f.thePoint = C;
				CtrlPoints.Add(f);
			}
		}
	}
}

/**
 * Gets dom team icon.
 * @param	TeamIndex	Zero-based index of the team.
 * @return	UTexture2D	the dom team icon.
 */
UTexture2D* UUTHUDWidget_DOMStatus::GetDomTeamIcon(uint8 TeamIndex) const
{
	return (DomTeamIconTexture.IsValidIndex(TeamIndex) ? DomTeamIconTexture[TeamIndex] : DomTeamIconTexture[4]);
}
