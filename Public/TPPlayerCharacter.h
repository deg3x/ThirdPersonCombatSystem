// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UTPSpringArmComponent;
class UBoxComponent;
class ATPEnemyBase;
class UTPPHealthComponent;

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	None,
	Forward,
	Backward,
	Right,
	Left
};

USTRUCT(BlueprintType)
struct FAnimProperties
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EMovementDirection MovementDirection;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool IsSprinting;
};

UENUM()
enum class EPlayerMovementMode : uint8
{
	Travelling,
	Combat
};

UCLASS()
class THIRDPERSON_API ATPPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTPSpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* WeaponRightBoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* WeaponLeftBoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTPPHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraFOVDefault = 85.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraFOVSprint = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraFOVInterpTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraAutoCenterInactivityTime = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Movement Variables")
	float MovementSpeed = 0.65f;

	UPROPERTY(EditAnywhere, Category = "Movement Variables")
	float RunningModifier = 1.7f;

	UPROPERTY(EditAnywhere, Category = "Movement Variables")
	float MinimumAcceleration = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Movement Variables")
	float TimeAccelerating = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Movement Variables")
	float TimeDecelerating = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Movement Variables")
	float PlayerRotationSpeed = 380.0f;

	UPROPERTY(EditAnywhere, Category = "Movement Variables")
	float RotationSmoothness = 1.3f;

	UPROPERTY(EditAnywhere, Category = "Combat Variables")
	float WeaponDamage = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Combat Variables")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, Category = "Combat Variables")
	float HitAnimationDelay = 0.085f;

	UPROPERTY(EditAnywhere, Category = "Combat Variables")
	float HitReducedPlayRate = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Combat Variables")
	float EnemyDistanceSnapLimit = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Combat Variables")
	float EnemySnapSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Combat Variables")
	float LookAtEnemySpeed = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Combat Variables")
	float CameraToEnemyMaxAngleToSnap = 40.0f;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UParticleSystem* SuccessfulHitFX;

	UPROPERTY(BlueprintReadOnly, Category = "Animation Variables")
	FAnimProperties AnimationVariables;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackResetMontage;

	float InputMovementAxisX;
	float InputMovementAxisY;
	float InputCameraAxisPitch;
	float InputCameraAxisYaw;
	float InputControllerCameraAxisPitch;
	float InputControllerCameraAxisYaw;
	float NormalizedAcceleration;
	float MovementAcceleration;
	float FOVInterpValue;
	float SprintModifier;
	float MovementInactivityTimer;
	float CameraInactivityTimer;

	bool IsAttacking;
	bool HasCachedAttack;
	bool CanCacheAttack;
	bool IsResettingAttack;
	bool IsLeftAxeOverlap;
	bool IsRightAxeOverlap;
	bool IsCombatCameraEnabled;
	bool IsCameraAutoCenterEnabled;
	bool IsInitiatingAttack;
	bool IsSnappingMovement;
	bool IsSnappingRotation;

	FName StartAttackSection;
	FVector LastMovementVector; // World space vector, preserved in accel/decel
	FVector LastInputAxis;		// Input Axis local space vector
	FRotator MeshToMovementRotator;
	UAnimInstance* AnimInstance;
	EPlayerMovementMode MovementMode;
	FTimerHandle HitDelayTimerHandle;
	ATPEnemyBase* LeftAxeOverlapEnemy;
	ATPEnemyBase* RightAxeOverlapEnemy;
	ATPEnemyBase* CurrentSelectedEnemy;

public:
	ATPPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	void ParsePlayerInput();
	void UpdateCameraValues() const;
	void MovePlayer(float DeltaTime);
	void MovePlayerCombat(float DeltaTime);
	void MovePlayerTravelling(float DeltaTime);
	void AlignMeshToMovement(float DeltaTime);
	void AlignMeshToLookDirection(float DeltaTime) const;
	void AlignMeshToMovementAndLookDirection(float DeltaTime);
	void AlignMeshToAttackTarget(float DeltaTime);
	void AdaptCameraFOVSprint(float DeltaTime);
	void HandlePlayerAttack();
	void PlayAttackResetMontage(int32 ResetSectionID);
	void ActionSprintPressed();
	void ActionSprintReleased();
	void ActionAttackPressed();
	void ActionCombatCamera();
	void PauseMontageOnHit();
	void BindAxeDelegates();
	void AdjustTimers(float DeltaTime);
	void CheckSetCameraAutoCenterEnabled();

	UFUNCTION()
	void HandleAxeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void HandleAxeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void UnpauseMontageAfterHit();

	FVector GetMeshForwardVector() const;
	FVector InputAxisToMovementVector();
	FRotator MeshToMovementRotation(float Angle, float RotationDirection);
	FRotator MeshToCameraInterpRotation(float Angle, float RotationDirection, float DeltaTime) const;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void CheckForEnemyHit(bool IsLeftHit);
	void SetCurrentEnemyBoss(ATPEnemyBase* Boss);
	void SetCombatCameraEnabled(bool IsEnabled);
	void CheckActiveEnemyKilled(ATPEnemyBase* KilledEnemy);
};
