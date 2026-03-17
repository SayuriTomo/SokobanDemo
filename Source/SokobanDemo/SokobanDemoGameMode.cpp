// Copyright Epic Games, Inc. All Rights Reserved.

#include "SokobanDemoGameMode.h"
#include "SokobanDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASokobanDemoGameMode::ASokobanDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
