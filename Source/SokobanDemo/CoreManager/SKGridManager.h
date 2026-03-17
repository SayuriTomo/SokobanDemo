// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SKTypes.h"
#include "GameFramework/PlayerStart.h"
#include "SKGridManager.generated.h"

class ASKBoxActor;
class ASKDoorActor;
class ACharacter;

UCLASS()
class SOKOBANDEMO_API ASKGridManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKGridManager();

	// Grid Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	int32 Width = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	int32 Height = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	float CellSize = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FVector GridOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	bool bDrawDebugGrid = true;

	// Spawn Classes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TSubclassOf<AActor> FloorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TSubclassOf<AActor> WallClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TSubclassOf<AActor> GoalClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TSubclassOf<AActor> IceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TSubclassOf<AActor> OneWayGateClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TSubclassOf<ASKDoorActor> DoorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TSubclassOf<ASKBoxActor> BoxClass;

	// Runtime Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Grid")
	TMap<FIntPoint,FSKCellData> CellMap;

	UPROPERTY()
	TMap<FIntPoint, ASKBoxActor*> BoxMap;
	
	UPROPERTY()
	TArray<ASKDoorActor*> DoorActors;

	UPROPERTY()
	TArray<AActor*> SpawnedLevelActors;

	// Grid Utils
	UFUNCTION(BlueprintCallable, Category="Grid")
	FVector GridToWorld(FIntPoint GridPos) const;

	UFUNCTION(BlueprintCallable, Category="Grid")
	FIntPoint WorldToGrid(FVector WorldPos) const;

	UFUNCTION(BlueprintCallable, Category="Grid")
	bool IsInside(FIntPoint GridPos) const;

	UFUNCTION(BlueprintCallable, Category="Grid")
	void InitializeEmptyGrid(ESKCellType DefaultType = ESKCellType::Empty);

	UFUNCTION(BlueprintCallable, Category="Grid")
	void ResizeGrid(int32 NewWidth, int32 NewHeight, bool bClearOldData = false);

	UFUNCTION(BlueprintCallable, Category="Grid")
	void DrawDebugGrid() const;

	// Query
	UFUNCTION(BlueprintCallable, Category="Grid")
	bool HasBox(FIntPoint GridPos) const;

	UFUNCTION(BlueprintCallable, Category="Grid")
	ASKBoxActor* GetBox(FIntPoint GridPos);

	UFUNCTION(BlueprintCallable, Category="Grid")
	bool IsGoal(FIntPoint GridPos) const;

	UFUNCTION(BlueprintCallable, Category="Grid")
	bool IsIce(FIntPoint GridPos) const;
	
	UFUNCTION(BlueprintCallable, Category="Grid")
	ASKDoorActor* GetDoorAt(FIntPoint GridPos) const;

	UFUNCTION(BlueprintCallable, Category="Grid")
	void CheckDoorAt(FIntPoint GridPos);
	
	// Box Management
	UFUNCTION(BlueprintCallable, Category="Grid")
	void RegisterBox(FIntPoint GridPos, ASKBoxActor* Box);

	UFUNCTION(BlueprintCallable, Category="Grid")
	void UnregisterBox(FIntPoint GridPos);

	UFUNCTION(BlueprintCallable, Category="Grid")
	void MoveBoxRecord(FIntPoint From, FIntPoint To, ASKBoxActor* Box);

	// Gameplay
	UFUNCTION(BlueprintCallable, Category="Grid")
	bool TryMoveCharacter(ACharacter* Character, FIntPoint From, FIntPoint To, ESKMoveDir MoveDir);

	UFUNCTION(BlueprintCallable, Category="Grid")
	bool TryPushBox(FIntPoint BoxFrom, FIntPoint BoxTo, ESKMoveDir MoveDir);
	
	UFUNCTION(BlueprintCallable, Category="Grid")
	void CheckGoalsAndDoors();

	UFUNCTION(BlueprintCallable, Category="Grid")
	bool IsCellWalkable(FIntPoint From, FIntPoint To, ESKMoveDir MoveDir, bool bForCharacter) const;
	
	// Level Data
	UFUNCTION(BlueprintCallable, Category="Level")
	FSKLevelData ExportLevelData() const;

	UFUNCTION(BlueprintCallable, Category="Level")
	void BuildLevelFromData(const FSKLevelData& LevelData);

	UFUNCTION(BlueprintCallable, Category="Level")
	void RebuildLevelActors();

	UFUNCTION(BlueprintCallable, Category="Level")
	void TravelToLevel(FName LevelName);

	UFUNCTION(BlueprintCallable, Category="Level")
	void RestartCurrentLevel();

	// Editor Load
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Editor Load")
	FString EditorLoadSlotName = TEXT("Level01");

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Editor Load")
	void LoadSavedLevelInEditor();

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Editor Load")
	void ClearSpawnedLevelActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Level Cache")
	bool bUseCachedLevelDataOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Level Cache")
	FSKLevelData CachedLevelData;

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Editor Load")
	void UpdateCachedLevelDataFromCurrentCellMap();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FIntPoint GetNextGridPos(FIntPoint From, ESKMoveDir MoveDir) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
