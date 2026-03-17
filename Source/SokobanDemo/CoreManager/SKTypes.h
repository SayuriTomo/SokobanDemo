#pragma once
#include "CoreMinimal.h"
#include "SKTypes.generated.h"

UENUM(BlueprintType)
enum class ESKCellType : uint8
{
	Empty       UMETA(DisplayName="Empty"),
	Floor       UMETA(DisplayName="Floor"),
	Wall        UMETA(DisplayName="Wall"),
	Goal        UMETA(DisplayName="Goal"),
	Door        UMETA(DisplayName="Door"),
	Ice         UMETA(DisplayName="Ice"),
	OneWayGate  UMETA(DisplayName="OneWayGate")
};

UENUM(BlueprintType)
enum class ESKBrushType : uint8
{
	Floor,
	Wall,
	Goal,
	Door,
	Ice,
	OneWayGate,
	Box,
	PlayerStart,
	Erase
};

UENUM(BlueprintType)
enum class ESKMoveDir : uint8
{
	Up,
	Down,
	Left,
	Right
};

USTRUCT(BlueprintType)
struct FSKCellData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint GridPos = FIntPoint::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESKCellType CellType = ESKCellType::Empty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasBox = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPlayerStart = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESKMoveDir GateAllowDir = ESKMoveDir::Up;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GroupID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DoorLevelName = NAME_None;
};

USTRUCT(BlueprintType)
struct FSKLevelData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Width = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Height = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CellSize = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector GridOrigin = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSKCellData> Cells;
};