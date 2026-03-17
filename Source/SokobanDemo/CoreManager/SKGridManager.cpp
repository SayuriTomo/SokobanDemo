// Fill out your copyright notice in the Description page of Project Settings.


#include "SKGridManager.h"
#include "Actors/SKBoxActor.h"
#include "Actors/SKDoorActor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "LevelEditor/SKLevelSaveGame.h"
#include "SokobanDemoCharacter.h"

// Sets default values
ASKGridManager::ASKGridManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

FVector ASKGridManager::GridToWorld(FIntPoint GridPos) const
{
	return GridOrigin + FVector(
			(GridPos.X + 0.5f) * CellSize,
			(GridPos.Y + 0.5f) * CellSize,
			0.f);
}

FIntPoint ASKGridManager::WorldToGrid(FVector WorldPos) const
{
	FVector Local = WorldPos - GridOrigin;

	return FIntPoint(
		FMath::FloorToInt(Local.X / CellSize),
		FMath::FloorToInt(Local.Y / CellSize)
	);
}

bool ASKGridManager::IsInside(FIntPoint GridPos) const
{
	return GridPos.X >= 0 && GridPos.X < Width && GridPos.Y >= 0 && GridPos.Y < Height;
}

void ASKGridManager::InitializeEmptyGrid(ESKCellType DefaultType)
{
	CellMap.Empty();
	BoxMap.Empty();

	for (AActor* Actor : SpawnedLevelActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
	SpawnedLevelActors.Empty();
	DoorActors.Empty();

	for (int32 X = 0; X < Width; ++X)
	{
		for (int32 Y = 0; Y < Height; ++Y)
		{
			FSKCellData Cell;
			Cell.GridPos = FIntPoint(X, Y);
			Cell.CellType = DefaultType;
			Cell.bHasBox = false;
			Cell.bIsPlayerStart = false;
			Cell.GateAllowDir = ESKMoveDir::Up;

			CellMap.Add(Cell.GridPos, Cell);
		}
	}

	RebuildLevelActors();
}

void ASKGridManager::ResizeGrid(int32 NewWidth, int32 NewHeight, bool bClearOldData)
{
	Width = FMath::Max(1, NewWidth);
	Height = FMath::Max(1, NewHeight);

	if (bClearOldData)
	{
		InitializeEmptyGrid(ESKCellType::Empty);
		return;
	}

	TArray<FIntPoint> KeysToRemove;

	for (const TPair<FIntPoint, FSKCellData>& Pair : CellMap)
	{
		if (!IsInside(Pair.Key))
		{
			KeysToRemove.Add(Pair.Key);
		}
	}

	for (const FIntPoint& Key : KeysToRemove)
	{
		CellMap.Remove(Key);
		BoxMap.Remove(Key);
	}

	RebuildLevelActors();
}

void ASKGridManager::DrawDebugGrid() const
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (int32 X = 0; X <= Width; ++X)
	{
		const FVector Start = GridOrigin + FVector(X * CellSize, 0.f, 2.f);
		const FVector End   = GridOrigin + FVector(X * CellSize, Height * CellSize, 2.f);
		DrawDebugLine(World, Start, End, FColor::Green, false, -1.f, 0, 1.5f);
	}

	for (int32 Y = 0; Y <= Height; ++Y)
	{
		const FVector Start = GridOrigin + FVector(0.f, Y * CellSize, 2.f);
		const FVector End   = GridOrigin + FVector(Width * CellSize, Y * CellSize, 2.f);
		DrawDebugLine(World, Start, End, FColor::Green, false, -1.f, 0, 1.5f);
	}
}

void ASKGridManager::RebuildLevelActors()
{
    for (AActor* Actor : SpawnedLevelActors)
    {
        if (Actor)
        {
            Actor->Destroy();
        }
    }

    SpawnedLevelActors.Empty();
    BoxMap.Empty();
    DoorActors.Empty();

    UWorld* World = GetWorld();
    if (!World) return;

	FActorSpawnParameters SpawnParams;
#if WITH_EDITOR
	SpawnParams.ObjectFlags |= RF_Transactional;
#endif

	for (const TPair<FIntPoint, FSKCellData>& Pair : CellMap)
	{
		const FIntPoint GridPos = Pair.Key;
		const FSKCellData& Cell = Pair.Value;

		if (!IsInside(GridPos))
		{
			continue;
		}

		if (Cell.CellType == ESKCellType::Empty)
		{
			continue;
		}

		const FVector BaseLoc = GridToWorld(GridPos);
		
		if (FloorClass)
		{
			AActor* Floor = World->SpawnActor<AActor>(
				FloorClass,
				BaseLoc,
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (Floor)
			{
				SpawnedLevelActors.Add(Floor);
			}
		}

		switch (Cell.CellType)
		{
		case ESKCellType::Wall:
		{
			if (WallClass)
			{
				AActor* Wall = World->SpawnActor<AActor>(
					WallClass,
					BaseLoc,
					FRotator::ZeroRotator,
					SpawnParams
				);

				if (Wall)
				{
					SpawnedLevelActors.Add(Wall);
				}
			}
			break;
		}

		case ESKCellType::Goal:
		{
			if (GoalClass)
			{
				AActor* Goal = World->SpawnActor<AActor>(
					GoalClass,
					BaseLoc + FVector(0.f, 0.f, 2.f),
					FRotator::ZeroRotator,
					SpawnParams
				);

				if (Goal)
				{
					SpawnedLevelActors.Add(Goal);
				}
			}
			break;
		}

		case ESKCellType::Door:
		{
			if (DoorClass)
			{
				ASKDoorActor* Door = World->SpawnActor<ASKDoorActor>(
					DoorClass,
					BaseLoc,
					FRotator::ZeroRotator,
					SpawnParams
				);

				if (Door)
				{
					Door->DoorGroupID = Cell.GroupID;
					Door->TargetLevelName = Cell.DoorLevelName;
					Door->InitDoorBaseZ();
					SpawnedLevelActors.Add(Door);
					DoorActors.Add(Door);
				}
			}
			break;
		}

		case ESKCellType::Ice:
		{
			if (IceClass)
			{
				AActor* Ice = World->SpawnActor<AActor>(
					IceClass,
					BaseLoc + FVector(0.f, 0.f, 2.f),
					FRotator::ZeroRotator,
					SpawnParams
				);

				if (Ice)
				{
					SpawnedLevelActors.Add(Ice);
				}
			}
			break;
		}

		case ESKCellType::OneWayGate:
		{
			if (OneWayGateClass)
			{
				FRotator GateRot = FRotator::ZeroRotator;

				switch (Cell.GateAllowDir)
				{
				case ESKMoveDir::Up:
					GateRot = FRotator(0.f, 0.f, 0.f);
					break;

				case ESKMoveDir::Right:
					GateRot = FRotator(0.f, 90.f, 0.f);
					break;

				case ESKMoveDir::Down:
					GateRot = FRotator(0.f, 180.f, 0.f);
					break;

				case ESKMoveDir::Left:
					GateRot = FRotator(0.f, 270.f, 0.f);
					break;

				default:
					break;
				}

				AActor* Gate = World->SpawnActor<AActor>(
					OneWayGateClass,
					BaseLoc + FVector(0.f, 0.f, 2.f),
					GateRot,
					SpawnParams
				);

				if (Gate)
				{
					SpawnedLevelActors.Add(Gate);
				}
			}
			break;
		}

		default:
			break;
		}

		// Player Start
		if (Cell.bIsPlayerStart)
		{
			APlayerStart* PlayerStartActor = World->SpawnActor<APlayerStart>(
				APlayerStart::StaticClass(),
				BaseLoc + FVector(0.f, 0.f, 20.f),
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (PlayerStartActor)
			{
				SpawnedLevelActors.Add(PlayerStartActor);
			}
		}

		// Box
		if (Cell.bHasBox && BoxClass)
		{
			ASKBoxActor* Box = World->SpawnActor<ASKBoxActor>(
				BoxClass,
				BaseLoc,
				FRotator::ZeroRotator,
				SpawnParams
			);

			if (Box)
			{
				Box->InitBox(this, GridPos);
				SpawnedLevelActors.Add(Box);
			}
		}
	}
	
	for (const TPair<FIntPoint, FSKCellData>& Pair : CellMap)
	{
		if (Pair.Value.bIsPlayerStart)
		{
			ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			if (ASokobanDemoCharacter* SKPlayer = Cast<ASokobanDemoCharacter>(Player))
			{
				SKPlayer->SnapToGridPos(Pair.Key);
			}
			break;
		}
	}

	CheckGoalsAndDoors();
}

void ASKGridManager::TravelToLevel(FName LevelName)
{
	UE_LOG(LogTemp, Warning, TEXT("Succcccccccccccccccc"));
	if (LevelName.IsNone())
	{
		return;
	}
	UGameplayStatics::OpenLevel(this, LevelName);
}

void ASKGridManager::RestartCurrentLevel()
{
	const FName CurrentLevel = FName(*UGameplayStatics::GetCurrentLevelName(this));

	UGameplayStatics::OpenLevel(this, CurrentLevel);
}

void ASKGridManager::LoadSavedLevelInEditor()
{
	if (EditorLoadSlotName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("EditorLoadSlotName is empty."));
		return;
	}

	if (!UGameplayStatics::DoesSaveGameExist(EditorLoadSlotName, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGame slot not found: %s"), *EditorLoadSlotName);
		return;
	}

	USKLevelSaveGame* SaveObj = Cast<USKLevelSaveGame>(
		UGameplayStatics::LoadGameFromSlot(EditorLoadSlotName, 0)
	);

	if (!SaveObj)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load save game object from slot: %s"), *EditorLoadSlotName);
		return;
	}
#if WITH_EDITOR
	Modify();
#endif

	// Load from Cached 
	CachedLevelData = SaveObj->SavedLevelData;
	CachedLevelData.GridOrigin = GetActorLocation();
	
	BuildLevelFromData(CachedLevelData);
	
#if WITH_EDITOR
	if (UWorld* World = GetWorld())
	{
		if (World->PersistentLevel)
		{
			World->PersistentLevel->Modify();
		}
		World->MarkPackageDirty();
	}
#endif
	
	UE_LOG(LogTemp, Warning, TEXT("Loaded saved level in editor from slot: %s"), *EditorLoadSlotName);
}

void ASKGridManager::ClearSpawnedLevelActors()
{
	for (AActor* Actor : SpawnedLevelActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
	CellMap.Empty();
	SpawnedLevelActors.Empty();
	BoxMap.Empty();
	DoorActors.Empty();

	UpdateCachedLevelDataFromCurrentCellMap();
}

FIntPoint ASKGridManager::GetNextGridPos(FIntPoint From, ESKMoveDir MoveDir) const
{
	switch (MoveDir)
	{
	case ESKMoveDir::Up:
		return FIntPoint(From.X + 1, From.Y);

	case ESKMoveDir::Down:
		return FIntPoint(From.X - 1, From.Y);

	case ESKMoveDir::Left:
		return FIntPoint(From.X, From.Y - 1);

	case ESKMoveDir::Right:
		return FIntPoint(From.X, From.Y + 1);

	default:
		return From;
	}
}

bool ASKGridManager::HasBox(FIntPoint GridPos) const
{
	return BoxMap.Contains(GridPos) && BoxMap[GridPos] != nullptr;
}

ASKBoxActor* ASKGridManager::GetBox(FIntPoint GridPos)
{
	if (ASKBoxActor** Found = BoxMap.Find(GridPos))
	{
		return *Found;
	}
	return nullptr;
}

bool ASKGridManager::TryMoveCharacter(ACharacter* Character, FIntPoint From, FIntPoint To, ESKMoveDir MoveDir)
{
	if (!Character) return false;

	if (!IsInside(To)) return false;

	if (HasBox(To))
	{
		const FIntPoint BoxTarget = GetNextGridPos(To, MoveDir);

		if (!TryPushBox(To, BoxTarget, MoveDir))
		{
			return false;
		}
	}
	else
	{
		if (!IsCellWalkable(From, To, MoveDir, true))
		{
			return false;
		}
			
	}

	FVector NewWorld = GridToWorld(To);
	Character->SetActorLocation(NewWorld);
	
	FIntPoint Current = To;
	while (IsIce(Current))
	{
		FIntPoint Next = GetNextGridPos(Current, MoveDir);

		if (HasBox(Next))
		{
			break;
		}

		if (!IsCellWalkable(Current,Next, MoveDir, true))
		{
			break;
		}

		Current = Next;
		Character->SetActorLocation(GridToWorld(Current));
		if (ASokobanDemoCharacter* SKPlayer = Cast<ASokobanDemoCharacter>(Character))
		{
			SKPlayer->GridPos = Current;
		}
	}
	CheckDoorAt(Current);

	return true;
}

bool ASKGridManager::TryPushBox(FIntPoint BoxFrom, FIntPoint BoxTo, ESKMoveDir MoveDir)
{
	ASKBoxActor* Box = GetBox(BoxFrom);
	if (!Box) return false;

	if (!IsCellWalkable(BoxFrom, BoxTo, MoveDir, false))
		return false;

	MoveBoxRecord(BoxFrom, BoxTo, Box);
	Box->MoveToGrid(BoxTo);
	
	FIntPoint Current = BoxTo;
	
	while (IsIce(Current))
	{
		FIntPoint Next = GetNextGridPos(Current, MoveDir);

		if (HasBox(Next))
		{
			break;
		}

		if (!IsCellWalkable(Current,Next, MoveDir, true))
		{
			break;
		} 

		MoveBoxRecord(Current, Next, Box);
		Current = Next;
		Box->MoveToGrid(Current);
	}

	CheckGoalsAndDoors();
	return true;
}

bool ASKGridManager::IsGoal(FIntPoint GridPos) const
{
	const FSKCellData* Cell = CellMap.Find(GridPos);
	return Cell && Cell->CellType == ESKCellType::Goal;
}

bool ASKGridManager::IsIce(FIntPoint GridPos) const
{
	const FSKCellData* Cell = CellMap.Find(GridPos);
	return Cell && Cell->CellType == ESKCellType::Ice;
}

void ASKGridManager::CheckDoorAt(FIntPoint GridPos)
{
	UE_LOG(LogTemp, Warning, TEXT("Succcccccccccccccccc"));
	const FSKCellData* Cell = CellMap.Find(GridPos);

	if (!Cell)
	{
		return;
	}

	if (Cell->CellType != ESKCellType::Door)
	{
		return;
	}

	ASKDoorActor* Door = GetDoorAt(GridPos);

	if (!Door)
	{
		return;
	}

	if (!Door->CanTravelToNextLevel())
	{
		return;
	}

	TravelToLevel(Door->TargetLevelName);
}

ASKDoorActor* ASKGridManager::GetDoorAt(FIntPoint GridPos) const
{
	for (ASKDoorActor* Door: DoorActors)
	{
		if (!Door)
		{
			continue;
		}
		const FIntPoint DoorPos = WorldToGrid(Door->GetActorLocation());

		if (DoorPos == GridPos)
		{
			return Door;
		}
	}
	return nullptr;
}

bool ASKGridManager::IsCellWalkable(FIntPoint From, FIntPoint To, ESKMoveDir MoveDir, bool bForCharacter) const
{
	if (!IsInside(To))
	{
		return false;
	}

	const FSKCellData* FromCell = CellMap.Find(From);
	const FSKCellData* ToCell = CellMap.Find(To);

	if (!ToCell)
	{
		return false;
	}
	
	if (FromCell && FromCell->CellType == ESKCellType::OneWayGate)
	{
		ESKMoveDir BlockedExitDir = ESKMoveDir::Up;

		switch (FromCell->GateAllowDir)
		{
		case ESKMoveDir::Up:
			BlockedExitDir = ESKMoveDir::Down;
			break;

		case ESKMoveDir::Down:
			BlockedExitDir = ESKMoveDir::Up;
			break;

		case ESKMoveDir::Left:
			BlockedExitDir = ESKMoveDir::Right;
			break;

		case ESKMoveDir::Right:
			BlockedExitDir = ESKMoveDir::Left;
			break;

		default:
			break;
		}

		if (MoveDir == BlockedExitDir)
		{
			return false;
		}
	}
	
	if (ToCell->CellType == ESKCellType::Wall)
	{
		return false;
	}

	if (ToCell->CellType == ESKCellType::Door)
	{
		bool bDoorOpen = false;

		for (ASKDoorActor* Door : DoorActors)
		{
			if (!Door)
			{
				continue;
			}

			// Door 位置可能有Z偏移，所以只比较网格坐标
			if (WorldToGrid(Door->GetActorLocation()) == To)
			{
				bDoorOpen = Door->bOpen;
				break;
			}
		}

		if (!bDoorOpen)
		{
			return false;
		}
	}

	if (ToCell->CellType == ESKCellType::OneWayGate)
	{
		if (MoveDir != ToCell->GateAllowDir)
		{
			return false;
		}
	}
	
	if (HasBox(To))
	{
		return false;
	}

	return true;
}

void ASKGridManager::RegisterBox(FIntPoint GridPos, ASKBoxActor* Box)
{
	BoxMap.Add(GridPos, Box);
}

void ASKGridManager::UnregisterBox(FIntPoint GridPos)
{
	BoxMap.Remove(GridPos);
}

void ASKGridManager::MoveBoxRecord(FIntPoint From, FIntPoint To, ASKBoxActor* Box)
{
	BoxMap.Remove(From);
	BoxMap.Add(To, Box);
}

void ASKGridManager::CheckGoalsAndDoors()
{
	TMap<int32, int32> GoalCount;
	TMap<int32, int32> GoalFilled;

	for (const TPair<FIntPoint, FSKCellData>& Pair : CellMap)
	{
		const FSKCellData& Cell = Pair.Value;

		if (Cell.CellType != ESKCellType::Goal)
			continue;

		int32 Group = Cell.GroupID;

		GoalCount.FindOrAdd(Group)++;

		if (HasBox(Pair.Key))
		{
			GoalFilled.FindOrAdd(Group)++;
		}
	}

	for (ASKDoorActor* Door : DoorActors)
	{
		if (!Door)
			continue;

		int32 Group = Door->DoorGroupID;

		bool bOpen = false;

		if (GoalCount.Contains(Group))
		{
			int32 Total = GoalCount[Group];
			int32 Filled = GoalFilled.FindRef(Group);

			bOpen = (Total > 0 && Total == Filled);
		}

		Door->SetDoorOpen(bOpen);
	}
}

void ASKGridManager::BuildLevelFromData(const FSKLevelData& LevelData)
{
	Width = LevelData.Width;
	Height = LevelData.Height;
	CellSize = LevelData.CellSize;
	GridOrigin = LevelData.GridOrigin;
	
	CellMap.Empty();
	BoxMap.Empty();
	DoorActors.Empty();

	for (const FSKCellData& Cell : LevelData.Cells)
	{
		CellMap.Add(Cell.GridPos, Cell);
	}
	
	RebuildLevelActors();
	CheckGoalsAndDoors();
}

FSKLevelData ASKGridManager::ExportLevelData() const
{
	FSKLevelData Data;
	Data.Width = Width;
	Data.Height = Height;
	Data.CellSize = CellSize;
	Data.GridOrigin = GridOrigin;

	for (const TPair<FIntPoint, FSKCellData>& Pair : CellMap)
	{
		FSKCellData Cell = Pair.Value;
		Cell.bHasBox = HasBox(Pair.Key);
		Data.Cells.Add(Cell);
	}

	return Data;
}

void ASKGridManager::UpdateCachedLevelDataFromCurrentCellMap()
{
	CachedLevelData = ExportLevelData();
	bUseCachedLevelDataOnBeginPlay = true;
}

// Called when the game starts or when spawned
void ASKGridManager::BeginPlay()
{
	Super::BeginPlay();

	GridOrigin = GetActorLocation();

	if (bUseCachedLevelDataOnBeginPlay && CachedLevelData.Cells.Num() > 0)
	{
		FSKLevelData RuntimeData = CachedLevelData;
		
		RuntimeData.GridOrigin = GetActorLocation();

		BuildLevelFromData(RuntimeData);
	}
	
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (ASokobanDemoCharacter* SKPlayer = Cast<ASokobanDemoCharacter>(Player))
	{
		SKPlayer->GridManager = this;
		SKPlayer->GridPos = WorldToGrid(SKPlayer->GetActorLocation());
	}
}

// Called every frame
void ASKGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDrawDebugGrid)
	{
		DrawDebugGrid();
	}
}

