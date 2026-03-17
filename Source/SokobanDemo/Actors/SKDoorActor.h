// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SKDoorActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USceneComponent;

UCLASS()
class SOKOBANDEMO_API ASKDoorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKDoorActor();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BlockCollider;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Door")
	float OpenOffsetZ = 500.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Door")
	float ClosedZ = 0.f;

	UFUNCTION(BlueprintCallable)
	void InitDoorBaseZ();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Door")
	int32 DoorGroupID = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Door")
	bool bOpen = false;

	UFUNCTION(BlueprintCallable)
	void SetDoorOpen(bool bInOpen);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	FName TargetLevelName = NAME_None;

	bool bTravelTriggered = false;
	
	UFUNCTION(BlueprintCallable)
	bool CanTravelToNextLevel() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
