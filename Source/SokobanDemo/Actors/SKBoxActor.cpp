// Fill out your copyright notice in the Description page of Project Settings.

#include "SKBoxActor.h"
#include "Components/StaticMeshComponent.h"
#include "CoreManager/SKGridManager.h"

// Sets default values
ASKBoxActor::ASKBoxActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void ASKBoxActor::InitBox(ASKGridManager* InGridManager, FIntPoint InGridPos)
{
	GridManager = InGridManager;
	GridPos = InGridPos;

	if (GridManager)
	{
		SetActorLocation(GridManager->GridToWorld(GridPos));
		GridManager->RegisterBox(GridPos, this);
	}
}

void ASKBoxActor::MoveToGrid(FIntPoint NewGridPos)
{
	if (!GridManager)
	{
		return;
	}
	
	GridPos = NewGridPos;

	FVector NewLocation = GridManager->GridToWorld(NewGridPos);

	SetActorLocation(NewLocation);
}

// Called when the game starts or when spawned
void ASKBoxActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASKBoxActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

