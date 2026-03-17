// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CoreManager/SKTypes.h"
#include "SKLevelSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class SOKOBANDEMO_API USKLevelSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	FSKLevelData SavedLevelData;
};
