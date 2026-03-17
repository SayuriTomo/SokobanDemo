// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "CoreManager/SKTypes.h"
#include "SokobanDemoCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ASKGridManager;
class ASKLevelEditorManager;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ASokobanDemoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	/** Move Input Action */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//UInputAction* MoveAction;

	/** Look Input Action */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//UInputAction* LookAction;

	/** Move Up Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveUpAction;

	/** Move Down Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveDownAction;

	/** Move Left Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveLeftAction;

	/** Move Right Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveRightAction;

	/** Zoom Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ZoomAction;
	
public:
	ASokobanDemoCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Editor")
	ASKLevelEditorManager* EditorManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	ASKGridManager* GridManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FIntPoint GridPos = FIntPoint::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	bool bInEditorMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	bool bSnapToGridOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move")
	bool bCanMove = true;

protected:
	virtual void BeginPlay() override;
	
	/** Called for movement input */
	//void Move(const FInputActionValue& Value);

	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();

	void ZoomCamera(const FInputActionValue& Value);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float ZoomSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MinCameraArmLength = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MaxCameraArmLength = 2500.0f;

	/** Called for looking input */
	//void Look(const FInputActionValue& Value);

	void TryStep(ESKMoveDir Dir);

	FIntPoint GetNextPos(FIntPoint From, ESKMoveDir Dir) const;

	void FaceDirection(ESKMoveDir Dir);

	UFUNCTION(BlueprintCallable, Category="Grid")
	void RefreshGridPosition();

public:
	UFUNCTION(BlueprintCallable, Category="Grid")
	void SetGridManager(ASKGridManager* InGridManager);

	UFUNCTION(BlueprintCallable, Category="Editor")
	void SetEditorMode(bool bEnableEditorMode);

	UFUNCTION(BlueprintCallable, Category="Grid")
	void SnapToGridPos(FIntPoint NewGridPos);
	
protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

