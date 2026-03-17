// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SKBoxActor.generated.h"

class UStaticMeshComponent;
class ASKGridManager;

UCLASS()
class SOKOBANDEMO_API ASKBoxActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKBoxActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint GridPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ASKGridManager* GridManager;

	UFUNCTION(BlueprintCallable)
	void InitBox(ASKGridManager* InGridManager, FIntPoint InGridPos);

	UFUNCTION(BlueprintCallable)
	void MoveToGrid(FIntPoint NewGridPos);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
