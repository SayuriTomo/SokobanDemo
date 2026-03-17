// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoreManager/SKTypes.h"
#include "SKLevelEditorManager.generated.h"

class ASKGridManager;
class ASokobanDemoCharacter;

UCLASS()
class SOKOBANDEMO_API ASKLevelEditorManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKLevelEditorManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// References
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Editor")
	ASKGridManager* GridManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Editor")
	ASokobanDemoCharacter* PlayerCharacter;
	
	// Editor State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Editor")
	bool bEditorEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Editor")
	ESKBrushType CurrentBrush = ESKBrushType::Wall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Editor")
	bool bEnableContinuousPaint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Editor")
	bool bDrawHoverDebug = true;

	UPROPERTY(BlueprintReadOnly, Category="Editor")
	FIntPoint HoverGridPos = FIntPoint::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Editor")
	int32 CurrentGroupID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Editor")
	ESKMoveDir CurrentGateDir = ESKMoveDir::Up;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Editor")
	FName CurrentDoorLevelName = NAME_None;

	// Editor Mode
	UFUNCTION(BlueprintCallable, Category="Editor")
	void ToggleEditor();

	UFUNCTION(BlueprintCallable, Category="Editor")
	void SetEditorEnabled(bool bEnabled);

	// Brush
	UFUNCTION(BlueprintCallable, Category="Editor")
	void SetBrush(ESKBrushType NewBrush);

	UFUNCTION(BlueprintCallable, Category="Editor")
	void PaintCell(FIntPoint GridPos, bool bErase);
	
	UFUNCTION(BlueprintCallable, Category="Editor")
	void SetCurrentGroupID(int32 NewGroupID);

	UFUNCTION(BlueprintCallable, Category="Editor")
	void SetCurrentGateDir(ESKMoveDir NewDir);

	UFUNCTION(BlueprintCallable, Category="Editor")
	void SetCurrentDoorLevelName(const FString& NewLevelName);

	// Grid
	UFUNCTION(BlueprintCallable, Category="Editor")
	void CreateNewGrid(int32 NewWidth, int32 NewHeight, ESKCellType DefaultType = ESKCellType::Empty);

	UFUNCTION(BlueprintCallable, Category="Editor")
	void ResizeGrid(int32 NewWidth, int32 NewHeight, bool bClearOldData = false);

	// Save / Load
	UFUNCTION(BlueprintCallable, Category="Editor")
	void SaveLevel(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category="Editor")
	void LoadLevel(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category="Editor")
	bool GetMouseGridPosition(FIntPoint& OutGridPos) const;
	
protected:
	void HandleMouseInput();
	
	void ApplyBrushToCell(FSKCellData& Cell);

	void DrawHoverCellDebug() const;
};
