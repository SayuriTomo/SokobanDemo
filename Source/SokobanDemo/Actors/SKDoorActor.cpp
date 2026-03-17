// Fill out your copyright notice in the Description page of Project Settings.


#include "SKDoorActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "SokobanDemoCharacter.h"
#include "SKGridManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASKDoorActor::ASKDoorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void ASKDoorActor::InitDoorBaseZ()
{
	ClosedZ = GetActorLocation().Z;
}

void ASKDoorActor::SetDoorOpen(bool bInOpen)
{
	bOpen = bInOpen;
	
	FVector Loc = GetActorLocation();

	if (bOpen)
	{
		Loc.Z = ClosedZ + OpenOffsetZ;
	}
	else
	{
		Loc.Z = ClosedZ;
	}

	SetActorLocation(Loc);
}

bool ASKDoorActor::CanTravelToNextLevel() const
{
	return bOpen && !TargetLevelName.IsNone();
}

// Called when the game starts or when spawned
void ASKDoorActor::BeginPlay()
{
	Super::BeginPlay();

	ClosedZ = GetActorLocation().Z;
	
}

// Called every frame
void ASKDoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

