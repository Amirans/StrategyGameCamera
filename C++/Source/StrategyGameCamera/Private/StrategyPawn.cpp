// Fill out your copyright notice in the Description page of Project Settings.

#include "StrategyPawn.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AStrategyPawn::AStrategyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	// Creating Player Base
	PawnBaseCollsion = CreateDefaultSubobject<USphereComponent>(TEXT("PawnBase"));
	PawnBaseCollsion->InitSphereRadius(32.0f);
	PawnBaseCollsion->SetCollisionProfileName(TEXT("Pawn"));

	RootComponent = PawnBaseCollsion;

	//Creating Spring Arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetWorldRotation(FRotator(-50.0f,0.0f, 0.0f));
	SpringArm->TargetArmLength = 800.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->bDoCollisionTest = false;

	//Create Camera and Setup attachment
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AStrategyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//Cache Movement Speed
	DefaultCameraMovementSpeed = CameraMovementSpeed;

	//Cache Player Controller
	PlayerController = Cast<APlayerController>(Controller);

	if (PlayerController)
		PlayerController->bShowMouseCursor = true;
}

void AStrategyPawn::HandleEdgeScrolling()
{

	if (!PlayerController)
		return;

	float MouseX = 0.0f;
	float MouseY = 0.0f;

	if (!PlayerController->GetMousePosition(MouseX, MouseY))
		return;

	int32 SizeX = 0;
	int32 SizeY = 0;

	PlayerController->GetViewportSize(SizeX, SizeY);

	if (MouseX < EdgeScrollingSensitivity) //Mouse is Close to Left
	{
		AddRight(-1);
	}
	else if (MouseX > SizeX - EdgeScrollingSensitivity) //Mouse is Close to Right
	{
		AddRight(1);
	}

	if (MouseY < EdgeScrollingSensitivity) //Mouse is Close to Top
	{
		AddForward(1);
	}
	else if (MouseY > SizeY - EdgeScrollingSensitivity) //Mouse is Close to Bottom
	{
		AddForward(-1);
	}

}

void AStrategyPawn::HandleCameraRotation()
{
	if (!PlayerController)
		return;

	float MouseXDelta = 0.0f;
	float MouseYDelta = 0.0f;

	PlayerController->GetInputMouseDelta(MouseXDelta, MouseYDelta);

	PawnBaseCollsion->AddRelativeRotation(FRotator(0.0f, MouseXDelta * CameraRotationSpeed, 0.0f));

	
	float SpringArmY = SpringArm->RelativeRotation.Pitch;

	SpringArmY += MouseYDelta * CameraRotationSpeed;

	SpringArm->SetRelativeRotation(FRotator(FMath::Clamp(SpringArmY,-85.0f,-35.0f), 0.0f, 0.0f));

}

void AStrategyPawn::OnCameraRotation()
{
	bIsRotatingCamera = true;

	if (PlayerController)
		PlayerController->CurrentMouseCursor = EMouseCursor::GrabHandClosed;
}

void AStrategyPawn::OnCameraRotationReleased()
{
	bIsRotatingCamera = false;

	if (PlayerController)
		PlayerController->CurrentMouseCursor = EMouseCursor::Default;
}

// Called every frame
void AStrategyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRotatingCamera)
		HandleCameraRotation();
	else
		HandleEdgeScrolling();

}

// Called to bind functionality to input
void AStrategyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AStrategyPawn::AddForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AStrategyPawn::AddRight);

	PlayerInputComponent->BindAction("IncreaseCameraSpeed", IE_Pressed, this, &AStrategyPawn::OnIncreaseCameraSpeed);
	PlayerInputComponent->BindAction("IncreaseCameraSpeed", IE_Released, this, &AStrategyPawn::OnIncreaseCameraSpeedReleased);

	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &AStrategyPawn::OnCameraZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &AStrategyPawn::OnCameraZoomOut);

	PlayerInputComponent->BindAction("RotateCamera", IE_Pressed, this, &AStrategyPawn::OnCameraRotation);
	PlayerInputComponent->BindAction("RotateCamera", IE_Released, this, &AStrategyPawn::OnCameraRotationReleased);

}

void AStrategyPawn::AddForward(float Axis)
{
	FVector NewDeltaLocation = PawnBaseCollsion->GetForwardVector() * (CameraMovementSpeed * Axis);
	AddActorWorldOffset(NewDeltaLocation);
}

void AStrategyPawn::AddRight(float Axis)
{
	FVector NewDeltaLocation = PawnBaseCollsion->GetRightVector() * (CameraMovementSpeed * Axis);
	AddActorWorldOffset(NewDeltaLocation);
}

void AStrategyPawn::OnIncreaseCameraSpeed()
{
	CameraMovementSpeed = DefaultCameraMovementSpeed * 2;
}

void AStrategyPawn::OnIncreaseCameraSpeedReleased()
{
	CameraMovementSpeed = DefaultCameraMovementSpeed;
}

void AStrategyPawn::OnCameraZoomIn()
{
	float Interval = bInvertedZoom ? 1 : -1;

	Interval *= ScrollSensitivity;

	SpringArm->TargetArmLength = FMath::Clamp(Interval + SpringArm->TargetArmLength, MaximumZoom, MinimumZoom);

	HandleZoomToPosition(Interval);

}

void AStrategyPawn::OnCameraZoomOut()
{
	float Interval = bInvertedZoom ? -1 : 1;

	Interval *= ScrollSensitivity;

	SpringArm->TargetArmLength = FMath::Clamp(Interval + SpringArm->TargetArmLength, MaximumZoom, MinimumZoom);

	HandleZoomToPosition(Interval);

}

void AStrategyPawn::HandleZoomToPosition(float ZoomValue)
{
	if ((!bZoomToMouse) || ZoomValue > 0 || SpringArm->TargetArmLength <= MaximumZoom)
		return;

	if (!PlayerController)
		return;

	float MouseX = 0.0f;
	float MouseY = 0.0f;

	PlayerController->GetMousePosition(MouseX, MouseY);

	FVector2D CurrentMousePos = FVector2D(MouseX, MouseY);

	if (!LastMousePosition.Equals(CurrentMousePos, 20.0f))
	{
		FVector MouseWorldPosition = FVector::ZeroVector;
		FVector MouseWorldDirection = FVector::OneVector;

		if (PlayerController->DeprojectMousePositionToWorld(MouseWorldPosition, MouseWorldDirection))
			StartZoomPosition = MouseWorldPosition + (MouseWorldDirection * SpringArm->TargetArmLength);

	}

	LastMousePosition = CurrentMousePos;

	SetActorLocation(FVector(StartZoomPosition.X, StartZoomPosition.Y, 0), false);
}
