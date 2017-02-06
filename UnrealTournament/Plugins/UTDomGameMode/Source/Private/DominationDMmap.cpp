// Created by Brian 'Snake' Alexander, 2016
#include "UnrealTournament.h"
#include "ControlPoint.h"
#include "Domination.h"
#include "Private/Slate/Widgets/SUTTabWidget.h"
#include "SNumericEntryBox.h"
#include "DominationDMmap.h"

ADominationDMmap::ADominationDMmap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = NSLOCTEXT("UTDomGameMode", "DOMDM", "Domination (DM maps)");
	MapPrefix = TEXT("DM");
	MaxControlPoints = 3;
}

void ADominationDMmap::PreInitializeComponents()
{
	if (GetWorld()->GetMapName().StartsWith("DM-"))
	{
		AutoGenerateControlPoints();
	}
	Super::PreInitializeComponents();
}

void ADominationDMmap::AutoGenerateControlPoints()
{
	AUTRecastNavMesh* NavData = GetUTNavData(GetWorld());
	TArray<AActor*> SomeActor;
	TArray<AControlPoint*> SpawnedPoints;

	if (NavData != NULL)
	{
		for (TActorIterator<AUTPickup> PickObjIt(GetWorld()); PickObjIt; ++PickObjIt)
		{
			if (!SomeActor.Contains(*PickObjIt))
			{
				SomeActor.AddUnique(*PickObjIt);
			}
		}
		// Incase we need more
		if (SomeActor.Num() < 1)
		{
			for (TActorIterator<APlayerStart> ObjIt(GetWorld()); ObjIt; ++ObjIt)
			{
				if (!SomeActor.Contains(*ObjIt))
				{
					SomeActor.AddUnique(*ObjIt);
				}
			}
		}
		int32 RandomActorIndex = FMath::RandRange(0, SomeActor.Num());
		for (int32 SpawnedCount = 0; SpawnedCount < MaxControlPoints; SpawnedCount++)
		{
			if (SpawnedCount != 0)
			{
				RandomActorIndex = FMath::RandRange(0, SomeActor.Num());
			}
			const FVector MyLoc = SomeActor[RandomActorIndex]->GetActorLocation();
			const UUTPathNode* MyNode = NavData->FindNearestNode(MyLoc, NavData->GetPOIExtent(SomeActor[RandomActorIndex]));

			TArray<const UUTPathNode*> AllNodes = NavData->GetAllNodes();
			// sort nodes by overall area viability
			AllNodes.Sort([](const UUTPathNode& A, const UUTPathNode& B)
			{
				// rate based on number of linked paths and number of polys in this node (implying area size)
				// TODO: calculate actual poly area?
				float RatingA = 0.5f / FMath::Max<int32>(1, A.Paths.Num()) + 0.5f / FMath::Max<int32>(1, A.Polys.Num());
				float RatingB = 0.5f / FMath::Max<int32>(1, B.Paths.Num()) + 0.5f / FMath::Max<int32>(1, A.Polys.Num());
				return RatingA > RatingB;
			});
			struct FPotentialControlPoint
			{
				FVector Loc;
				float Distance;
			};
			TArray<FPotentialControlPoint> Choices;
			int32 NumNodesWithChoices = 0;
			for (const UUTPathNode* Node : AllNodes)
			{
				bool bGotChoice = false;
				if (Node != MyNode)
				{
					for (NavNodeRef TestPoly : Node->Polys)
					{
						const FVector TestLoc = NavData->GetPolyCenter(TestPoly) + FVector(0.0f, 0.0f, NavData->GetPOIExtent(NULL).Z);
						if (!GetWorld()->LineTraceTestByChannel(MyLoc, TestLoc, ECC_Pawn, FCollisionQueryParams(), WorldResponseParams))
						{
							new(Choices) FPotentialControlPoint{ TestLoc, (TestLoc - MyLoc).Size() };
							if (!bGotChoice)
							{
								NumNodesWithChoices++;
								bGotChoice = true;
							}
						}
					}
					// bail once we have a reasonable variety of options
					if (NumNodesWithChoices > 6)
					{
						break;
					}
				}
			}
			Choices.Sort([](const FPotentialControlPoint& A, const FPotentialControlPoint& B) { return A.Distance > B.Distance; });
			Choices.SetNum(FMath::Min<int32>(Choices.Num(), 6));

			for (const FPotentialControlPoint& Pt : Choices)
			{
				if (SpawnedPoints.Num() == MaxControlPoints) return;

				bool bSkipThis = false;
				// Prevent spawning too close to other ControlPoint
				if (SpawnedPoints.IsValidIndex(0) && SpawnedPoints[0] != nullptr && SpawnedPoints.Num() > 0)
				{
					for (uint8 i = 0; i < SpawnedPoints.Num(); i++)
					{
						if ((SpawnedPoints[i]->GetActorLocation() - Pt.Loc).Size() < 2000.f)
						{
							bSkipThis = true;
							break;
						}
					}
				}
				if (!bSkipThis)
				{
					FActorSpawnParameters Params;
					Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					AControlPoint* NewPoint = GetWorld()->SpawnActor<AControlPoint>(Pt.Loc + FVector(0, 0, -70), FRotator(0, 0, 0), Params);
					if (NewPoint != NULL)
					{
						SpawnedPoints.Add(NewPoint);
						NewPoint->PointName = FString::FromInt(SpawnedPoints.Num());
						NewPoint->TeamNum = 4;
						if (SpawnedPoints.Num() == MaxControlPoints) return;

						break;
					}
				}
			}
		}
	}
}

#if !UE_SERVER
void ADominationDMmap::CreateConfigWidgets(TSharedPtr<class SVerticalBox> MenuSpace, bool bCreateReadOnly, TArray< TSharedPtr<TAttributePropertyBase> >& ConfigProps, int32 MinimumPlayers)
{
	Super::CreateConfigWidgets(MenuSpace, bCreateReadOnly, ConfigProps, MinimumPlayers);

	TSharedPtr< TAttributeProperty<int32> > MaxControlPointsAttr = StaticCastSharedPtr<TAttributeProperty<int32>>(FindGameURLOption(ConfigProps, TEXT("MaxControlPoints")));

	if (MaxControlPointsAttr.IsValid())
	{
		MenuSpace->AddSlot()
			.AutoHeight()
			.VAlign(VAlign_Top)
			.Padding(0.0f, 0.0f, 0.0f, 5.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(350)
			[
				SNew(STextBlock)
				.TextStyle(SUWindowsStyle::Get(), "UT.Common.NormalText")
			.Text(NSLOCTEXT("UTDomGameMode", "NumControlPoints", "Control Points"))
			]
			]
		+ SHorizontalBox::Slot()
			.Padding(20.0f, 0.0f, 0.0f, 0.0f)
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(300)
			[
				bCreateReadOnly ?
				StaticCastSharedRef<SWidget>(
			SNew(STextBlock)
			.TextStyle(SUWindowsStyle::Get(), "UT.Common.ButtonText.White")
			.Text(MaxControlPointsAttr.ToSharedRef(), &TAttributeProperty<int32>::GetAsText)
			) :
			StaticCastSharedRef<SWidget>(
			SNew(SNumericEntryBox<int32>)
			.Value(MaxControlPointsAttr.ToSharedRef(), &TAttributeProperty<int32>::GetOptional)
			.OnValueChanged(MaxControlPointsAttr.ToSharedRef(), &TAttributeProperty<int32>::Set)
			.AllowSpin(true)
			.Delta(1)
			.MinValue(1)
			.MaxValue(7)
			.MinSliderValue(1)
			.MaxSliderValue(7)
			.EditableTextBoxStyle(SUWindowsStyle::Get(), "UT.Common.NumEditbox.White")
			)
			]
			]
			];
	}
}
#endif
