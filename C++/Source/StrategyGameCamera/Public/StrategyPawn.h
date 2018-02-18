// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "StrategyPawn.generated.h"

UCLASS()
class STRATEGYGAMECAMERA_API AStrategyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AStrategyPawn();

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = Components)
		USphereComponent* PawnBaseCollsion;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = Components)
		USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = Components)
		UCameraComponent* Camera;

	/* Movement Speed of the Camera*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraConfig)
		float CameraMovementSpeed = 10.0f;

	/* Maximum Zoom Value (Zoom In) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraConfig)
		float MaximumZoom = 500.0f;

	/* Minimum Zoom Value (Zoom Out) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraConfig)
		float MinimumZoom = 1000.0f;

	/* Scroll Sensitivity of the Zoom*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraConfig)
		float ScrollSensitivity = 15.0f;

	/* Determines if Zoom is Inverted*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraConfig)
		bool bInvertedZoom = false;

	/* Determines if Zoom is Inverted*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CameraConfig)
		bool bZoomToMouse = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Default Movement Speed*/
	float DefaultCameraMovementSpeed = 0.0f;

	/* Last Mouse Position*/
	FVector2D LastMousePosition = FVector2D::ZeroVector;

	/* Player Controller */
	APlayerController* PlayerController;

	/* Starting Zoom Position */
	FVector StartZoomPosition = FVector::ZeroVector;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Add World offset in Direction of Pawn Base Forward Vector*/
	UFUNCTION(BlueprintCallable, Category = Movement)
	void AddForward(float Axis);

	/* Add World offset in Direction of Pawn Base Right Vector*/
	UFUNCTION(BlueprintCallable, Category = Movement)
	void AddRight(float Axis);
	
	/* Increase Camera Speed by Double*/
	UFUNCTION(BlueprintCallable, Category = Movement)
	void OnIncreaseCameraSpeed();
	
	/* Resets Camera Speed to Default*/
	UFUNCTION(BlueprintCallable, Category = Movement)
	void OnIncreaseCameraSpeedReleased();

	/* Zooms in the Camera */
	UFUNCTION(BlueprintCallable, Category = Movement)
	void OnCameraZoomIn();

	/* Zooms out the Camera */
	UFUNCTION(BlueprintCallable, Category = Movement)
	void OnCameraZoomOut();

	/* Move Camera to Position*/
	UFUNCTION(BlueprintCallable, Category = Movement)
	void HandleZoomToPosition(float ZoomValue);

};
