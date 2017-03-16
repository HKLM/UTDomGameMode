// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTSkinName.h"
#include "UTCharacter.h"
#include "UTDomCharacter.h"

AUTDomCharacter::AUTDomCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AUTDomCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AUTDomCharacter, TeamBodySkinColor, COND_None);
	DOREPLIFETIME_CONDITION(AUTDomCharacter, TeamSkinOverlayColor, COND_None);
}

void AUTDomCharacter::SetTeamSkin(uint8 NewTeamIndex)
{
	if (NewTeamIndex < 4)
	{
		/* value what base color to use (red or blue) */
		float FTeamNum = (NewTeamIndex == 1 || NewTeamIndex == 2) ? 1.0f : 0.0f;
		for (UMaterialInstanceDynamic* MIDom : GetBodyMIs())
		{
			if (MIDom)
			{
				MIDom->SetScalarParameterValue(NAME_UseTeamColors, 1.0f);
				MIDom->SetScalarParameterValue(NAME_TeamSelect, FTeamNum);
				if (NewTeamIndex == 1 || NewTeamIndex == 2) //Blue & Green Team
				{
					// We use blue team base skin also for the green team
					MIDom->SetVectorParameterValue(NAME_TeamBlueColor, TeamBodySkinColor);
					MIDom->SetVectorParameterValue(NAME_TeamBlueOverlay, TeamSkinOverlayColor);
					//merc
					MIDom->SetVectorParameterValue(NAME_MercMaleTeamBluePlastic, TeamBodySkinColor);
					MIDom->SetVectorParameterValue(NAME_MercMaleTeamBlueCamo, TeamBodySkinColor);
					MIDom->SetVectorParameterValue(NAME_MercMaleTeamBlueNylon, TeamBodySkinColor);
					//nec male
					MIDom->SetVectorParameterValue(NAME_NecMaleTeamBlueDarkPlastic, TeamBodySkinColor);
					MIDom->SetVectorParameterValue(NAME_NecMaleTeamBlueLatex, TeamBodySkinColor);
					//visse
					MIDom->SetVectorParameterValue(NAME_VisseTeamRubberTint, TeamBodySkinColor);
					//Skaarj
					MIDom->SetVectorParameterValue(NAME_SkaarjTeamBlueTintAlpha, TeamBodySkinColor);
				}
				else if (NewTeamIndex == 0 || NewTeamIndex == 3) //Red & Gold Team
				{
					// We use red team base skin also for the gold team, because there is no blue in the color yellow
					MIDom->SetVectorParameterValue(NAME_TeamRedColor, TeamBodySkinColor);
					MIDom->SetVectorParameterValue(NAME_TeamRedOverlay, TeamSkinOverlayColor);
					//merc
					MIDom->SetVectorParameterValue(NAME_MercMaleTeamRedPlastic, TeamBodySkinColor);
					MIDom->SetVectorParameterValue(NAME_MercMaleTeamRedCamo, TeamBodySkinColor);
					MIDom->SetVectorParameterValue(NAME_MercMaleTeamRedNylon, TeamBodySkinColor);
					//nec male
					MIDom->SetVectorParameterValue(NAME_NecMaleTeamRedDarkPlastic, TeamBodySkinColor);
					MIDom->SetVectorParameterValue(NAME_NecMaleTeamRedLatex, TeamBodySkinColor);
					//visse
					MIDom->SetVectorParameterValue(NAME_VisseTeamRubberTint, TeamBodySkinColor);
					//Skaarj
					MIDom->SetVectorParameterValue(NAME_SkaarjTeamRedTintAlpha, TeamBodySkinColor);
				}
			}
		}
	}
}
