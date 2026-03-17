// Copyright Epic Games, Inc. All Rights Reserved.

#include "SokobanDemoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "CoreManager/SKGridManager.h"
#include "LevelEditor/SKLevelEditorManager.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASokobanDemoCharacter

ASokobanDemoCharacter::ASokobanDemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1600.0f;
	CameraBoom->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 3.f;
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASokobanDemoCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASokobanDemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Move
		EnhancedInputComponent->BindAction(MoveUpAction, ETriggerEvent::Started, this, &ASokobanDemoCharacter::MoveUp);
		EnhancedInputComponent->BindAction(MoveDownAction, ETriggerEvent::Started, this, &ASokobanDemoCharacter::MoveDown);
		EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Started, this, &ASokobanDemoCharacter::MoveLeft);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Started, this, &ASokobanDemoCharacter::MoveRight);
		
		// Zoom
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ASokobanDemoCharacter::ZoomCamera);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASokobanDemoCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!GridManager)
	{
		GridManager = Cast<ASKGridManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ASKGridManager::StaticClass())
		);
	}
	
	if (!EditorManager)
	{
		EditorManager = Cast<ASKLevelEditorManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ASKLevelEditorManager::StaticClass())
		);
	}

	if (GridManager)
	{
		if (bSnapToGridOnBeginPlay)
		{
			GridPos = GridManager->WorldToGrid(GetActorLocation());
			SetActorLocation(GridManager->GridToWorld(GridPos));
		}
		else
		{
			GridPos = GridManager->WorldToGrid(GetActorLocation());
		}
	}
}

void ASokobanDemoCharacter::MoveUp()
{
	if (!GridManager)
	{
		return;
	}
	
	TryStep(ESKMoveDir::Up);
}

void ASokobanDemoCharacter::MoveDown()
{
	if (!GridManager)
	{
		return;
	}
	
	TryStep(ESKMoveDir::Down);
}

void ASokobanDemoCharacter::MoveLeft()
{
	if (!GridManager)
	{
		return;
	}
	
	TryStep(ESKMoveDir::Left);
}

void ASokobanDemoCharacter::MoveRight()
{
	if (!GridManager)
	{
		return;
	}
	
	TryStep(ESKMoveDir::Right);
}

void ASokobanDemoCharacter::ZoomCamera(const FInputActionValue& Value)
{
	float ZoomValue = Value.Get<float>();

	if (!CameraBoom)
	{
		return;
	}

	float NewArmLength = CameraBoom->TargetArmLength - ZoomValue * ZoomSpeed;

	NewArmLength = FMath::Clamp(NewArmLength, MinCameraArmLength, MaxCameraArmLength);

	CameraBoom->TargetArmLength = NewArmLength;
}

void ASokobanDemoCharacter::TryStep(ESKMoveDir Dir)
{
	if (!bCanMove || bInEditorMode || !GridManager)
	{
		return;
	}

	const FIntPoint Target = GetNextPos(GridPos, Dir);

	FaceDirection(Dir);

	if (GridManager->TryMoveCharacter(this, GridPos, Target, Dir))
	{
		GridPos = GridManager->WorldToGrid(GetActorLocation());
	}
}

FIntPoint ASokobanDemoCharacter::GetNextPos(FIntPoint From, ESKMoveDir Dir) const
{
	switch (Dir)
	{
	case ESKMoveDir::Up:
		return FIntPoint(From.X + 1, From.Y);

	case ESKMoveDir::Down:
		return FIntPoint(From.X - 1, From.Y );

	case ESKMoveDir::Left:
		return FIntPoint(From.X, From.Y - 1);

	case ESKMoveDir::Right:
		return FIntPoint(From.X, From.Y + 1);

	default:
		return From;
	}
}

void ASokobanDemoCharacter::FaceDirection(ESKMoveDir Dir)
{
	FRotator TargetRot = GetActorRotation();

	switch (Dir)
	{
	case ESKMoveDir::Up:
		TargetRot = FRotator(0.f, 0.f, 0.f);
		break;

	case ESKMoveDir::Down:
		TargetRot = FRotator(0.f, 180.f, 0.f);
		break;

	case ESKMoveDir::Left:
		TargetRot = FRotator(0.f, -90.f, 0.f);
		break;

	case ESKMoveDir::Right:
		TargetRot = FRotator(0.f, 90.f, 0.f);
		break;

	default:
		break;
	}

	SetActorRotation(TargetRot);
}

void ASokobanDemoCharacter::RefreshGridPosition()
{
	if (!GridManager)
	{
		return;
	}

	GridPos = GridManager->WorldToGrid(GetActorLocation());
}

void ASokobanDemoCharacter::SnapToGridPos(FIntPoint NewGridPos)
{
	GridPos = NewGridPos;

	if (GridManager)
	{
		SetActorLocation(GridManager->GridToWorld(GridPos));
	}
}

void ASokobanDemoCharacter::SetGridManager(ASKGridManager* InGridManager)
{
	GridManager = InGridManager;

	if (GridManager)
	{
		RefreshGridPosition();
	}
}

void ASokobanDemoCharacter::SetEditorMode(bool bEnableEditorMode)
{
	bInEditorMode = bEnableEditorMode;
}
