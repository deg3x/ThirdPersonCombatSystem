// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSpringArmComponent.h"
#include "GameFramework/Actor.h"

UTPSpringArmComponent::UTPSpringArmComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
	CombatTarget = nullptr;
	IsInSmoothZone = false;
}

void UTPSpringArmComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SelectSensitivity();
	CalculateIsInSmoothZone();
	RotateCameraBoomOutOfSmoothZone(DeltaTime);

	if (IsCombatModeEnabled && CombatTarget != nullptr)
	{
		RotateCameraBoomBasedOnEnemy(DeltaTime);
	}
	else
	{
		RotateCameraBoomBasedOnInput(DeltaTime);
	}

	if (IsAutoCenterEnabled)
	{
		if (IsCombatModeEnabled && CombatTarget != nullptr)
		{
			RotateCameraBoomAutoCenterPitch(DeltaTime);
		}
		else
		{
			RotateCameraBoomAutoCenter(DeltaTime);
		}
	}
}

void UTPSpringArmComponent::RotateCameraBoomBasedOnInput(float DeltaTime)
{
	float CameraPitch = YAxisValue * (CameraInvertY ? -1.0f : 1.0f);
	float CameraYaw = XAxisValue * (CameraInvertX ? -1.0f : 1.0f);

	FRotator NewCameraRotation = GetRelativeRotation();

	NewCameraRotation.Pitch = FMath::ClampAngle(NewCameraRotation.Pitch + CameraPitch * CameraSensitivity * DeltaTime, CameraPitchLimitNeg, CameraPitchLimitPos);
	NewCameraRotation.Yaw += CameraYaw * CameraSensitivity * DeltaTime;

	SetRelativeRotation(NewCameraRotation);
}

void UTPSpringArmComponent::RotateCameraBoomBasedOnEnemy(float DeltaTime)
{
	FVector CameraBoomForward = GetForwardVector();
	CameraBoomForward.Z = 0.0f;
	CameraBoomForward.Normalize();

	FVector TargetForward = CombatTarget->GetActorLocation() - GetComponentLocation();
	TargetForward.Z = 0.0f;
	TargetForward.Normalize();

	float Angle = FMath::Acos(FVector::DotProduct(CameraBoomForward, TargetForward)) * 180.0f / PI;
	float Direction = FVector::CrossProduct(CameraBoomForward, TargetForward).Z > 0.0f ? 1.0 : -1.0f;

	if (Angle < 2.5f)
	{
		Direction = 0.0f;
	}

	float CameraYaw = XAxisValue * (CameraInvertX ? -1.0f : 1.0f);
	float CameraPitch = YAxisValue * (CameraInvertY ? -1.0f : 1.0f);

	float YawSmoothness;
	if (Direction >= 0.0f)
	{
		YawSmoothness = 1.0f - FMath::Lerp(0.0f, 1.0f, (Angle / CameraYawLimitCombatPos));
	}
	else
	{
		YawSmoothness = 1.0f - FMath::Lerp(0.0f, 1.0f, -(Angle / CameraYawLimitCombatNeg));
	}

	YawSmoothness = FMath::Clamp(YawSmoothness, 0.1f, 1.0f);

	float CombatOffsetYawAmount = CameraYaw * DeltaTime * CameraSensitivityCombat * YawSmoothness;

	float EnemyFollowSpeed = Angle * 5.0f;
	float RotationAmount = EnemyFollowSpeed * DeltaTime * Direction;

	FRotator NewCameraRotation = GetRelativeRotation();
	NewCameraRotation.Pitch = FMath::ClampAngle(NewCameraRotation.Pitch + CameraPitch * CameraSensitivityCombat * DeltaTime, CameraPitchLimitCombatNeg, CameraPitchLimitCombatPos);
	NewCameraRotation.Yaw += RotationAmount + CombatOffsetYawAmount;
	
	SetRelativeRotation(NewCameraRotation);
}

void UTPSpringArmComponent::RotateCameraBoomAutoCenter(float DeltaTime)
{
	RotateCameraBoomAutoCenterYaw(DeltaTime);
	RotateCameraBoomAutoCenterPitch(DeltaTime);
}

void UTPSpringArmComponent::RotateCameraBoomAutoCenterPitch(float DeltaTime)
{
	FVector TargetForward = FVector::ForwardVector;
	FVector ForwardPitchOffset = TargetForward + GetForwardVector().Z;
	ForwardPitchOffset.Normalize();

	float Angle = FMath::Acos(FVector::DotProduct(ForwardPitchOffset, TargetForward)) * 180.0f / PI;
	float Direction = FVector::CrossProduct(ForwardPitchOffset, TargetForward).Y < 0.0f ? 1.0 : -1.0f;

	if (Angle < 1.0f)
	{
		return;
	}

	FRotator NewCameraRotation = GetRelativeRotation();

	float PitchAutoCenterSpeed = AutoCenterSpeed;
	if (LastYawOffsetAngle > Angle)
	{
		PitchAutoCenterSpeed *= Angle / LastYawOffsetAngle;
	}

	if (Angle < AutoCenterSmoothZoneDeg)
	{
		PitchAutoCenterSpeed = FMath::InterpEaseOut(0.0f, PitchAutoCenterSpeed, Angle / AutoCenterSmoothZoneDeg, 2);
	}
	
	NewCameraRotation.Pitch += PitchAutoCenterSpeed * DeltaTime * Direction;

	SetRelativeRotation(NewCameraRotation);
}

void UTPSpringArmComponent::RotateCameraBoomAutoCenterYaw(float DeltaTime)
{
	FVector CameraBoomForward = GetForwardVector();
	CameraBoomForward.Z = 0.0f;
	CameraBoomForward.Normalize();

	FVector TargetForward = PlayerForward;
	TargetForward.Z = 0.0f;
	TargetForward.Normalize();

	float Angle = FMath::Acos(FVector::DotProduct(CameraBoomForward, TargetForward)) * 180.0f / PI;
	float Direction = FVector::CrossProduct(CameraBoomForward, TargetForward).Z > 0.0f ? 1.0 : -1.0f;

	LastYawOffsetAngle = Angle;

	if (Angle < 1.0f)
	{
		return;
	}

	FRotator NewCameraRotation = GetRelativeRotation();

	float YawAutoCenterSpeed = AutoCenterSpeed;
	if (Angle < AutoCenterSmoothZoneDeg)
	{
		YawAutoCenterSpeed = FMath::InterpEaseOut(0.0f, YawAutoCenterSpeed, Angle / AutoCenterSmoothZoneDeg, 2);
	}

	float RotationAmount = YawAutoCenterSpeed * DeltaTime * Direction;

	NewCameraRotation.Yaw += RotationAmount;

	SetRelativeRotation(NewCameraRotation);
}

void UTPSpringArmComponent::RotateCameraBoomOutOfSmoothZone(float DeltaTime)
{
	if (!IsInSmoothZone)
	{
		return;
	}

	FRotator CameraBoomRotation = GetRelativeRotation();
	FRotator NewCameraRotation = GetRelativeRotation();

	float RotationAmount = PitchSmoothenFactor * CameraSensitivity * DeltaTime;
	RotationAmount *= CameraBoomRotation.Pitch < 0.0f ? 1.0f : -1.0f;
	
	NewCameraRotation.Pitch += RotationAmount;

	SetRelativeRotation(NewCameraRotation);
}

void UTPSpringArmComponent::CalculateIsInSmoothZone()
{
	FRotator CameraBoomRotation = GetRelativeRotation();
	float PitchLimitPos;
	float PitchLimitNeg;
	float PitchLimitExtended;

	if (IsCombatModeEnabled && CombatTarget != nullptr)
	{
		PitchLimitPos = CameraPitchLimitCombatPos;
		PitchLimitNeg = CameraPitchLimitCombatNeg;
		PitchLimitExtended = CameraPitchLimitCombatExtended;
	}
	else
	{
		PitchLimitPos = CameraPitchLimitPos;
		PitchLimitNeg = CameraPitchLimitNeg;
		PitchLimitExtended = CameraPitchLimitExtended;
	}

	if (CameraBoomRotation.Pitch > 0.0f)
	{
		IsInSmoothZone = (PitchLimitPos - PitchLimitExtended) - CameraBoomRotation.Pitch < 0.0f ? true : false;

		if (IsInSmoothZone)
		{
			float Alpha = (CameraBoomRotation.Pitch - (PitchLimitPos - PitchLimitExtended)) / PitchLimitExtended;
			Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
			PitchSmoothenFactor = FMath::InterpEaseIn(0.0f, 1.0f, Alpha, 2);
		}
		else
		{
			PitchSmoothenFactor = 0.0f;
		}
	}
	else
	{
		IsInSmoothZone = (PitchLimitNeg + PitchLimitExtended) - CameraBoomRotation.Pitch > 0.0f ? true : false;

		if (IsInSmoothZone)
		{
			float Alpha = ((PitchLimitNeg + PitchLimitExtended) - CameraBoomRotation.Pitch) / PitchLimitExtended;
			Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
			PitchSmoothenFactor = FMath::InterpEaseIn(0.0f, 1.0f, Alpha, 2);
		}
		else
		{
			PitchSmoothenFactor = 0.0f;
		}
	}
}

void UTPSpringArmComponent::SelectSensitivity()
{
	if (IsUsingController)
	{
		CameraSensitivity = CameraSensitivityController;
		CameraSensitivityCombat = CameraCombatSensitivityController;
	}
	else
	{
		CameraSensitivity = CameraSensitivityMouse;
		CameraSensitivityCombat = CameraCombatSensitivityMouse;
	}
}

void UTPSpringArmComponent::UpdateAxisValues(float XAxis, float YAxis)
{
	XAxisValue = XAxis;
	YAxisValue = YAxis;
}

void UTPSpringArmComponent::UpdateIsUsingController(bool IsControllerInUse)
{
	IsUsingController = IsControllerInUse;
}

void UTPSpringArmComponent::UpdateCombatModeEnabled(bool InCombat)
{
	IsCombatModeEnabled = InCombat;
}

void UTPSpringArmComponent::UpdateAutoCenterEnabled(bool IsEnabled)
{
	IsAutoCenterEnabled = IsEnabled;
}

void UTPSpringArmComponent::UpdateCombatTarget(AActor* Target)
{
	CombatTarget = Target;
}

void UTPSpringArmComponent::UpdatePlayerForward(FVector Forward)
{
	PlayerForward = Forward;
}

float UTPSpringArmComponent::GetCameraSensitivity()
{
	return CameraSensitivityMouse;
}

bool UTPSpringArmComponent::GetCameraInvertX()
{
	return CameraInvertX;
}

bool UTPSpringArmComponent::GetCameraInvertY()
{
	return CameraInvertY;
}