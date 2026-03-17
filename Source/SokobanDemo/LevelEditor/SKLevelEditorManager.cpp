// Fill out your copyright notice in the Description page of Project Settings.


#include "SKLevelEditorManager.h"

#include "CoreManager/SKGridManager.h"
#include "SKLevelSaveGame.h"
#include "SokobanDemoCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASKLevelEditorManager::ASKLevelEditorManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASKLevelEditorManager::BeginPlay()
{
	Super::BeginPlay();

	if (!GridManager)
	{
		GridManager = Cast<ASKGridManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ASKGridManager::StaticClass())
		);
	}

	if (!PlayerCharacter)
	{
		PlayerCharacter = Cast<ASokobanDemoCharacter>(
			UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
		);
	}
	
}

// Called every frame
void ASKLevelEditorManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bEditorEnabled)
	{
		return;
	}

	FIntPoint GridPos;
	if (GetMouseGridPosition(GridPos))
	{
		HoverGridPos = GridPos;
	}

	if (bDrawHoverDebug)
	{
		DrawHoverCellDebug();
	}

	HandleMouseInput();
}

void ASKLevelEditorManager::ToggleEditor()
{
	SetEditorEnabled(!bEditorEnabled);
}

void ASKLevelEditorManager::SetEditorEnabled(bool bEnabled)
{
	bEditorEnabled = bEnabled;

	if (PlayerCharacter)
	{
		PlayerCharacter->SetEditorMode(bEditorEnabled);
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->bShowMouseCursor = bEditorEnabled;

		if (bEditorEnabled)
		{
			PC->bShowMouseCursor = true;
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			PC->SetInputMode(InputMode);
		}
		else
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}

void ASKLevelEditorManager::SetBrush(ESKBrushType NewBrush)
{
	CurrentBrush = NewBrush;
}

void ASKLevelEditorManager::PaintCell(FIntPoint GridPos, bool bErase)
{
	if (!GridManager || !GridManager->IsInside(GridPos))
	{
		return;
	}

	FSKCellData* Existing = GridManager->CellMap.Find(GridPos);
	if (!Existing)
	{
		FSKCellData NewCell;
		NewCell.GridPos = GridPos;
		NewCell.CellType = ESKCellType::Empty;
		NewCell.bHasBox = false;
		NewCell.bIsPlayerStart = false;
		NewCell.GateAllowDir = ESKMoveDir::Up;

		GridManager->CellMap.Add(GridPos, NewCell);
		Existing = GridManager->CellMap.Find(GridPos);
	}

	if (!Existing)
	{
		return;
	}

	if (bErase || CurrentBrush == ESKBrushType::Erase)
	{
		Existing->CellType = ESKCellType::Empty;
		Existing->bHasBox = false;
		Existing->bIsPlayerStart = false;
	}
	else
	{
		if (CurrentBrush == ESKBrushType::PlayerStart)
		{
			for (TPair<FIntPoint, FSKCellData>& Pair : GridManager->CellMap)
			{
				Pair.Value.bIsPlayerStart = false;
			}
		}

		ApplyBrushToCell(*Existing);
	}

	GridManager->RebuildLevelActors();
}

void ASKLevelEditorManager::SetCurrentGroupID(int32 NewGroupID)
{
	CurrentGroupID = NewGroupID;
}

void ASKLevelEditorManager::SetCurrentGateDir(ESKMoveDir NewDir)
{
	CurrentGateDir = NewDir;
}

void ASKLevelEditorManager::SetCurrentDoorLevelName(const FString& NewLevelName)
{
	CurrentDoorLevelName = FName(NewLevelName);
}

void ASKLevelEditorManager::CreateNewGrid(int32 NewWidth, int32 NewHeight, ESKCellType DefaultType)
{
	if (!GridManager)
	{
		return;
	}

	GridManager->Width = FMath::Max(1, NewWidth);
	GridManager->Height = FMath::Max(1, NewHeight);
	GridManager->InitializeEmptyGrid(DefaultType);
}

void ASKLevelEditorManager::ResizeGrid(int32 NewWidth, int32 NewHeight, bool bClearOldData)
{
	if (!GridManager)
	{
		return;
	}

	GridManager->ResizeGrid(NewWidth, NewHeight, bClearOldData);
}

void ASKLevelEditorManager::SaveLevel(const FString& SlotName)
{
	if (!GridManager)
	{
		return;
	}

	USKLevelSaveGame* SaveObj = Cast<USKLevelSaveGame>(
		UGameplayStatics::CreateSaveGameObject(USKLevelSaveGame::StaticClass())
	);

	if (!SaveObj)
	{
		return;
	}

	SaveObj->SavedLevelData = GridManager->ExportLevelData();
	UGameplayStatics::SaveGameToSlot(SaveObj, SlotName, 0);
}

void ASKLevelEditorManager::LoadLevel(const FString& SlotName)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		return;
	}

	USKLevelSaveGame* SaveObj = Cast<USKLevelSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SlotName, 0)
	);

	if (!SaveObj || !GridManager)
	{
		return;
	}

	GridManager->BuildLevelFromData(SaveObj->SavedLevelData);
}

bool ASKLevelEditorManager::GetMouseGridPosition(FIntPoint& OutGridPos) const
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC || !GridManager)
	{
		return false;
	}

	float MouseX = 0.f;
	float MouseY = 0.f;
	if (!PC->GetMousePosition(MouseX, MouseY))
	{
		return false;
	}

	FVector WorldPos;
	FVector WorldDir;
	if (!PC->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldPos, WorldDir))
	{
		return false;
	}
	
	if (FMath::IsNearlyZero(WorldDir.Z))
	{
		return false;
	}

	const float T = (GridManager->GridOrigin.Z - WorldPos.Z) / WorldDir.Z;
	if (T < 0.f)
	{
		return false;
	}

	const FVector HitPos = WorldPos + WorldDir * T;
	OutGridPos = GridManager->WorldToGrid(HitPos);

	return GridManager->IsInside(OutGridPos);
}

void ASKLevelEditorManager::HandleMouseInput()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
	{
		return;
	}

	if (bEnableContinuousPaint)
	{
		if (PC->IsInputKeyDown(EKeys::LeftMouseButton))
		{
			PaintCell(HoverGridPos, false);
		}
		else if (PC->IsInputKeyDown(EKeys::RightMouseButton))
		{
			PaintCell(HoverGridPos, true);
		}
	}
	else
	{
		if (PC->WasInputKeyJustPressed(EKeys::LeftMouseButton))
		{
			PaintCell(HoverGridPos, false);
		}
		else if (PC->WasInputKeyJustPressed(EKeys::RightMouseButton))
		{
			PaintCell(HoverGridPos, true);
		}
	}
}

void ASKLevelEditorManager::ApplyBrushToCell(FSKCellData& Cell)
{
	switch (CurrentBrush)
	{
	case ESKBrushType::Floor:
		Cell.CellType = ESKCellType::Floor;
		break;

	case ESKBrushType::Wall:
		Cell.CellType = ESKCellType::Wall;
		Cell.bHasBox = false;
		Cell.bIsPlayerStart = false;
		break;

	case ESKBrushType::Goal:
		Cell.CellType = ESKCellType::Goal;
		Cell.GroupID = CurrentGroupID;
		break;

	case ESKBrushType::Door:
		Cell.CellType = ESKCellType::Door;
		Cell.bHasBox = false;
		Cell.GroupID = CurrentGroupID;
		Cell.DoorLevelName = CurrentDoorLevelName;
		break;

	case ESKBrushType::Ice:
		Cell.CellType = ESKCellType::Ice;
		break;

	case ESKBrushType::OneWayGate:
		Cell.CellType = ESKCellType::OneWayGate;
		Cell.GateAllowDir = CurrentGateDir;
		break;

	case ESKBrushType::Box:
		if (Cell.CellType == ESKCellType::Empty)
		{
			Cell.CellType = ESKCellType::Floor;
		}
		Cell.bHasBox = true;
		break;

	case ESKBrushType::PlayerStart:
		if (Cell.CellType == ESKCellType::Empty)
		{
			Cell.CellType = ESKCellType::Floor;
		}
		Cell.bIsPlayerStart = true;
		break;

	case ESKBrushType::Erase:
		Cell.CellType = ESKCellType::Empty;
		Cell.bHasBox = false;
		Cell.bIsPlayerStart = false;
		break;

	default:
		break;
	}
}

void ASKLevelEditorManager::DrawHoverCellDebug() const
{
	if (!GridManager || !bEditorEnabled)
	{
		return;
	}

	if (!GridManager->IsInside(HoverGridPos))
	{
		return;
	}

	const FVector Center = GridManager->GridToWorld(HoverGridPos) +
		FVector(0.f, 0.f, 5.f);

	const FVector Extent = FVector(
		GridManager->CellSize * 0.5f,
		GridManager->CellSize * 0.5f,
		5.f
	);

	DrawDebugBox(GetWorld(), Center, Extent, FColor::Yellow, false, -1.f, 0, 2.f);
}

