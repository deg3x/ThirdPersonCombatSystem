// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "TPSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UTPSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	bool CameraInvertX = false;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	bool CameraInvertY = true;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraSensitivityMouse = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraSensitivityController = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraCombatSensitivityMouse = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraCombatSensitivityController = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraPitchLimitPos = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraPitchLimitNeg = -50.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraPitchLimitExtended = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraPitchLimitCombatPos = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraPitchLimitCombatNeg = -20.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraYawLimitCombatPos = 35.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraYawLimitCombatNeg = -35.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraPitchLimitCombatExtended = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float AutoCenterSpeed = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float AutoCenterSmoothZoneDeg = 20.0f;

	float XAxisValue;
	float YAxisValue;
	float PitchSmoothenFactor;
	float CameraSensitivity;
	float CameraSensitivityCombat;
	float LastYawOffsetAngle;

	bool IsCombatModeEnabled;
	bool IsAutoCenterEnabled;
	bool IsUsingController;
	bool IsInSmoothZone;

	AActor* CombatTarget;
	FVector PlayerForward;

public:
	UTPSpringArmComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void RotateCameraBoomBasedOnInput(float DeltaTime);
	void RotateCameraBoomBasedOnEnemy(float DeltaTime);
	void RotateCameraBoomAutoCenter(float DeltaTime);
	void RotateCameraBoomAutoCenterPitch(float DeltaTime);
	void RotateCameraBoomAutoCenterYaw(float DeltaTime);
	void RotateCameraBoomOutOfSmoothZone(float DeltaTime);

	void CalculateIsInSmoothZone();
	void SelectSensitivity();

	void UpdateAxisValues(float XAxis, float YAxis);
	void UpdateIsUsingController(bool IsControllerInUse);
	void UpdateCombatModeEnabled(bool InCombat);
	void UpdateAutoCenterEnabled(bool IsEnabled);
	void UpdateCombatTarget(AActor* Target);
	void UpdatePlayerForward(FVector Forward);

	FORCEINLINE float GetCameraSensitivity();
	FORCEINLINE bool GetCameraInvertX();
	FORCEINLINE bool GetCameraInvertY();
};
