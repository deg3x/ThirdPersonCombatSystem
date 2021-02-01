// Fill out your copyright notice in the Description page of Project Settings.

#include "TPPlayerCharacter.h"
#include "TPSpringArmComponent.h"
#include "Camera\CameraComponent.h"
#include "Components\InputComponent.h"
#include "Components\SkeletalMeshComponent.h"
#include "Components\BoxComponent.h"
#include "Components\CapsuleComponent.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Animation\AnimInstance.h"
#include "Animation\AnimNode_StateMachine.h"
#include "Kismet\GameplayStatics.h"
#include "TimerManager.h"
#include "TPEnemyBase.h"
#include "TPPHealthComponent.h"

ATPPlayerCharacter::ATPPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<UTPSpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(CameraFOVDefault);

	HealthComponent = CreateDefaultSubobject<UTPPHealthComponent>(TEXT("HealthComponent"));

	WeaponRightBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponRightBoxComponent"));
	WeaponRightBoxComponent->SetupAttachment(GetMesh(), FName(TEXT("Weapon_Collider_R")));
	WeaponRightBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponRightBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponRightBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	WeaponLeftBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponLeftBoxComponent"));
	WeaponLeftBoxComponent->SetupAttachment(GetMesh(), FName(TEXT("Weapon_Collider_L")));
	WeaponLeftBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponLeftBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponLeftBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	AnimationVariables.MovementDirection = EMovementDirection::None;

	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;

	SprintModifier = 1.0f;
	MovementAcceleration = 0.0f;
	NormalizedAcceleration = 0.0f;
	FOVInterpValue = 0.0f;
	IsAttacking = false;
	HasCachedAttack = false;
	CanCacheAttack = false;
	StartAttackSection = TEXT("HitA_start");
	LastMovementVector = FVector::ZeroVector;
	MeshToMovementRotator = FRotator::ZeroRotator;
	MovementMode = EPlayerMovementMode::Travelling;
	CurrentSelectedEnemy = nullptr;
	IsCombatCameraEnabled = false;
	IsInitiatingAttack = false;
	IsSnappingMovement = false;
	IsSnappingRotation = false;
}

void ATPPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();

	BindAxeDelegates();
}

void ATPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ParsePlayerInput();
	CheckSetCameraAutoCenterEnabled();
	UpdateCameraValues();
	MovePlayer(DeltaTime);
	AdaptCameraFOVSprint(DeltaTime);
	AlignMeshToMovementAndLookDirection(DeltaTime);
	HandlePlayerAttack();
	AdjustTimers(DeltaTime);
}

void ATPPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward");
	PlayerInputComponent->BindAxis("MoveRight");
	PlayerInputComponent->BindAxis("LookUp");
	PlayerInputComponent->BindAxis("Turn");
	PlayerInputComponent->BindAxis("LookUpRate");
	PlayerInputComponent->BindAxis("TurnRate");
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ATPPlayerCharacter::ActionSprintPressed);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ATPPlayerCharacter::ActionSprintReleased);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &ATPPlayerCharacter::ActionAttackPressed);
	PlayerInputComponent->BindAction("CombatCamera", EInputEvent::IE_Pressed, this, &ATPPlayerCharacter::ActionCombatCamera);
}

void ATPPlayerCharacter::ParsePlayerInput()
{
	InputMovementAxisX = GetInputAxisValue("MoveForward");
	InputMovementAxisY = GetInputAxisValue("MoveRight");
	InputCameraAxisPitch = GetInputAxisValue("LookUp");
	InputCameraAxisYaw = GetInputAxisValue("Turn");
	InputControllerCameraAxisPitch = GetInputAxisValue("LookUpRate");
	InputControllerCameraAxisYaw = GetInputAxisValue("TurnRate");
}

void ATPPlayerCharacter::MovePlayer(float DeltaTime)
{
	AnimationVariables.MovementDirection = EMovementDirection::None;

	switch (MovementMode)
	{
		case EPlayerMovementMode::Travelling:
		{
			MovePlayerTravelling(DeltaTime);
			break;
		}
		case EPlayerMovementMode::Combat:
		{
			MovePlayerCombat(DeltaTime);
			break;
		}
	}
}

void ATPPlayerCharacter::MovePlayerCombat(float DeltaTime)
{
	MovementAcceleration = 0.0f;
	NormalizedAcceleration = 0.0f;
	LastInputAxis = FVector::ZeroVector;
	LastMovementVector = FVector::ZeroVector;

	if (CurrentSelectedEnemy == nullptr)
	{
		// What to do here?
	}
	else
	{
		if (!IsInitiatingAttack && !IsSnappingMovement && !IsSnappingRotation)
		{
			return;;
		}

		IsSnappingMovement = true;
		
		FVector CameraForward = CameraComponent->GetForwardVector();
		CameraForward.Z = 0.0f;
		CameraForward.Normalize();
		
		FVector CameraToEnemy = CurrentSelectedEnemy->GetActorLocation() - CameraComponent->GetComponentLocation();
		CameraToEnemy.Z = 0.0f;
		CameraToEnemy.Normalize();

		float Angle = FMath::Acos(FVector::DotProduct(CameraForward, CameraToEnemy)) * 180.0f / PI;

		if (Angle > CameraToEnemyMaxAngleToSnap)
		{
			IsSnappingMovement = false;
			return;
		}

		FVector EnemyLocation = CurrentSelectedEnemy->GetActorLocation();
		FVector DirectionToEnemy = EnemyLocation - GetActorLocation();
		float DistanceToEnemy = DirectionToEnemy.Size();
		
		if (DistanceToEnemy > EnemyDistanceSnapLimit)
		{
			IsSnappingMovement = false;
			return;
		}

		AlignMeshToAttackTarget(DeltaTime);

		float CloseDistanceLimit = GetCapsuleComponent()->GetScaledCapsuleRadius() + CurrentSelectedEnemy->GetCapsuleComponent()->GetScaledCapsuleRadius();
		CloseDistanceLimit += 280.0f;
		CloseDistanceLimit /= 2.0f;

		if (DistanceToEnemy <= CloseDistanceLimit)
		{
			IsSnappingMovement = false;
			return;
		}

		DirectionToEnemy.Normalize();

		FVector NewLocation = GetActorLocation() + (DeltaTime * EnemySnapSpeed * DirectionToEnemy);
		float NewDistanceToEnemy = (NewLocation - EnemyLocation).Size();

		if (NewDistanceToEnemy < (CloseDistanceLimit - 25.0f))
		{
			NewLocation *= (NewDistanceToEnemy / CloseDistanceLimit);
		}

		SetActorLocation(NewLocation);
	}
}

void ATPPlayerCharacter::MovePlayerTravelling(float DeltaTime)
{
	FVector MovementVector = InputAxisToMovementVector();

	NormalizedAcceleration += MovementVector.IsNearlyZero() ? -(DeltaTime / TimeDecelerating) : DeltaTime / TimeAccelerating;
	NormalizedAcceleration = FMath::Clamp(NormalizedAcceleration, 0.0f, 1.0f);
	float NormalizedAccelerationEaseIn = FMath::InterpEaseIn(0.0f, 1.0f, NormalizedAcceleration, 3.0f);

	if (NormalizedAcceleration == 0.0f)
	{
		LastMovementVector = FVector::ZeroVector;
		return;
	}

	MovementAcceleration = MinimumAcceleration + NormalizedAccelerationEaseIn * (1.0f - MinimumAcceleration);

	if (MovementVector.IsNearlyZero())
	{
		//SetActorLocation(GetActorLocation() + (LastMovementVector * MovementAcceleration * MovementSpeed * DeltaTime), true);
		AddMovementInput(LastMovementVector, MovementAcceleration * MovementSpeed);
		return;
	}

	MovementVector.Normalize();

	//SetActorLocation(GetActorLocation() + (MovementVector * MovementAcceleration * MovementSpeed * SprintModifier * DeltaTime), true);
	AddMovementInput(MovementVector, MovementAcceleration * MovementSpeed * SprintModifier);
	LastMovementVector = MovementVector;
}

void ATPPlayerCharacter::UpdateCameraValues() const
{
	if (InputControllerCameraAxisPitch + InputControllerCameraAxisYaw != 0.0f)
	{
		SpringArmComponent->UpdateAxisValues(InputControllerCameraAxisYaw, InputControllerCameraAxisPitch);
		SpringArmComponent->UpdateIsUsingController(true);
	}
	else
	{
		SpringArmComponent->UpdateAxisValues(InputCameraAxisYaw, InputCameraAxisPitch);
		SpringArmComponent->UpdateIsUsingController(false);
	}
	SpringArmComponent->UpdateCombatModeEnabled(IsCombatCameraEnabled);
	SpringArmComponent->UpdateCombatTarget(CurrentSelectedEnemy);
	SpringArmComponent->UpdatePlayerForward(GetMeshForwardVector());
	SpringArmComponent->UpdateAutoCenterEnabled(IsCameraAutoCenterEnabled);
}

void ATPPlayerCharacter::AlignMeshToMovement(float DeltaTime)
{
	if (LastInputAxis.IsNearlyZero())
	{
		return;
	}
	
	float Angle = FMath::Acos(FVector::DotProduct(FVector::ForwardVector, LastInputAxis)) * 180.0f / PI;
	float RotationDirection = (FVector::CrossProduct(FVector::ForwardVector, LastInputAxis).Z >= 0.0f ? 1.0f : -1.0f);

	MeshToMovementRotator = MeshToMovementRotation(Angle, RotationDirection);
}

void ATPPlayerCharacter::AlignMeshToLookDirection(float DeltaTime) const
{
	if (LastInputAxis.IsNearlyZero())
	{
		return;
	}

	FVector TargetForward = CameraComponent->GetForwardVector();
	FVector PlayerForward = GetMeshForwardVector();

	TargetForward.Z = 0.0f;
	PlayerForward.Z = 0.0f;
	TargetForward.Normalize();
	PlayerForward.Normalize();

	float Angle = FMath::Acos(FVector::DotProduct(PlayerForward, TargetForward)) * 180.0f / PI;

	if (Angle < 0.1f)
	{
		return;
	}

	float RotationDirection = FVector::CrossProduct(PlayerForward, TargetForward).Z >= 0.0f ? 1.0f : -1.0f;

	GetMesh()->AddLocalRotation(MeshToCameraInterpRotation(Angle, RotationDirection, DeltaTime));
}

void ATPPlayerCharacter::AdaptCameraFOVSprint(float DeltaTime)
{
	if (SprintModifier == 1.0f || LastMovementVector.IsNearlyZero())
	{
		FOVInterpValue -= DeltaTime / CameraFOVInterpTime;
	}
	else
	{
		FOVInterpValue += DeltaTime / CameraFOVInterpTime;
	}

	FOVInterpValue = FMath::Clamp(FOVInterpValue, 0.0f, 1.0f);
	float SmoothInterpValue = FMath::InterpEaseIn(0.0f, 1.0f, FOVInterpValue, 2.0f);
	CameraComponent->SetFieldOfView(FMath::Lerp(CameraFOVDefault, CameraFOVSprint, SmoothInterpValue));
}

void ATPPlayerCharacter::AlignMeshToMovementAndLookDirection(float DeltaTime)
{
	AlignMeshToMovement(DeltaTime);
	AlignMeshToLookDirection(DeltaTime);
}

void ATPPlayerCharacter::AlignMeshToAttackTarget(float DeltaTime)
{
	FVector ForwardVector = GetMesh()->GetRightVector();
	FVector TargetForward = CurrentSelectedEnemy->GetActorLocation() - GetActorLocation();
	TargetForward.Normalize();

	float Angle = FMath::Acos(FVector::DotProduct(ForwardVector, TargetForward)) * 180.0f / PI;
	float Direction = FVector::CrossProduct(ForwardVector, TargetForward).Z > 0.0f ? 1.0f : -1.0f;

	IsSnappingRotation = true;

	if (Angle < 7.0f)
	{
		IsSnappingRotation = false;
		return;
	}

	FRotator NewPlayerRotation = GetMesh()->GetRelativeRotation();
	float RotationAmount = DeltaTime * Direction * LookAtEnemySpeed;

	NewPlayerRotation.Yaw += RotationAmount;

	GetMesh()->SetRelativeRotation(NewPlayerRotation);
}

void ATPPlayerCharacter::ActionSprintPressed()
{
	AnimationVariables.IsSprinting = true;
	SprintModifier = RunningModifier;
}

void ATPPlayerCharacter::ActionSprintReleased()
{
	AnimationVariables.IsSprinting = false;
	SprintModifier = 1.0f;
}

void ATPPlayerCharacter::ActionAttackPressed()
{
	if (!IsAttacking || CanCacheAttack)
	{
		HasCachedAttack = true;
	}
}

void ATPPlayerCharacter::ActionCombatCamera()
{
	IsCombatCameraEnabled = !IsCombatCameraEnabled;
}

void ATPPlayerCharacter::HandlePlayerAttack()
{
	if (AnimInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[!] Animation Instance not valid!"));
		return;
	}

	if (!AnimInstance->IsAnyMontagePlaying())
	{
		IsAttacking = false;
		MovementMode = EPlayerMovementMode::Travelling;

		if (HasCachedAttack)
		{
			PlayAnimMontage(AttackMontage, 1.0f, StartAttackSection);
			IsAttacking = true;
			HasCachedAttack = false;
			MovementMode = EPlayerMovementMode::Combat;
		}
		else
		{
			StartAttackSection = AttackMontage->GetSectionName(0);
		}
	}
	else
	{
		FAnimMontageInstance* RunningMontage = AnimInstance->GetActiveMontageInstance();
		CanCacheAttack = false;

		if (RunningMontage)
		{
			FName SectionName = RunningMontage->GetCurrentSection();
			float SectionIndex;
			float SectionStartTime;
			float SectionEndTime;
			float NormalizedPosition;

			if (AnimInstance->GetCurrentActiveMontage() == AttackResetMontage)
			{
				SectionIndex = AttackResetMontage->GetSectionIndex(SectionName);
				AttackResetMontage->GetSectionStartAndEndTime(SectionIndex, SectionStartTime, SectionEndTime);
				NormalizedPosition = (RunningMontage->GetPosition() - SectionStartTime);
				NormalizedPosition /= AttackResetMontage->GetSectionLength(SectionIndex) - AttackResetMontage->BlendOut.GetBlendTime();

				if (NormalizedPosition > 0.25f)
				{
					IsAttacking = false;
					MovementMode = EPlayerMovementMode::Travelling;

					if (LastMovementVector != FVector::ZeroVector)
					{
						AnimInstance->StopAllMontages(0.2f);
					}
				}
			}
			else if (AnimInstance->GetCurrentActiveMontage() == AttackMontage)
			{
				SectionIndex = AttackMontage->GetSectionIndex(SectionName);
				AttackMontage->GetSectionStartAndEndTime(SectionIndex, SectionStartTime, SectionEndTime);
				NormalizedPosition = (RunningMontage->GetPosition() - SectionStartTime);
				NormalizedPosition /= AttackMontage->GetSectionLength(SectionIndex) - AttackMontage->BlendOut.GetBlendTime();

				if (NormalizedPosition < 0.1f)
				{
					IsInitiatingAttack = true;
				}
				else
				{
					IsInitiatingAttack = false;
				}

				if (NormalizedPosition > 0.25f)
				{
					CanCacheAttack = true;
				}

				if (SectionIndex == 2)
				{
					CanCacheAttack = false;
					HasCachedAttack = false;
					RunningMontage->SetNextSectionID(SectionIndex, -1);
				}

				if (NormalizedPosition > 0.95f && RunningMontage->GetNextSectionID(SectionIndex) == -1)
				{
					CanCacheAttack = false;
					PlayAttackResetMontage(SectionIndex);
				}

				if (HasCachedAttack)
				{
					HasCachedAttack = false;
					RunningMontage->SetNextSectionID(SectionIndex, SectionIndex + 1);
				}
			}
		}
	}
}

void ATPPlayerCharacter::PlayAttackResetMontage(int32 ResetSectionID)
{
	float StartTime;
	float EndTime;
	AttackResetMontage->GetSectionStartAndEndTime(ResetSectionID, StartTime, EndTime);
	AnimInstance->Montage_Play(AttackResetMontage, 1.0f, EMontagePlayReturnType::Duration, StartTime);

	IsResettingAttack = true;
}

// Always use this to get mesh forward
FVector ATPPlayerCharacter::GetMeshForwardVector() const
{
	// Rotate the forward vector of the mesh by the mesh to movement alignment
	FRotator MeshToMoveOffset = MeshToMovementRotator;
	MeshToMoveOffset.Yaw = -MeshToMoveOffset.Yaw;
	FVector Ret = MeshToMoveOffset.RotateVector(GetMesh()->GetRightVector());
	return Ret;
}

FVector ATPPlayerCharacter::InputAxisToMovementVector()
{
	LastInputAxis = FVector(InputMovementAxisX, InputMovementAxisY, 0.0f);
	LastInputAxis.Normalize();

	FVector ForwardVector = CameraComponent->GetForwardVector() * InputMovementAxisX;
	FVector RightVector = CameraComponent->GetRightVector() * InputMovementAxisY;
	FVector MovementVector = ForwardVector + RightVector;

	MovementVector.Z = 0.0f;

	return MovementVector;
}

FRotator ATPPlayerCharacter::MeshToMovementRotation(float Angle, float RotationDirection)
{
	FRotator MeshToMoveRotation = FRotator::ZeroRotator;

	if (Angle < 80.0f)
	{
		AnimationVariables.MovementDirection = EMovementDirection::Forward;
		MeshToMoveRotation.Yaw = RotationDirection * Angle;
	}
	else if (Angle < 140.0f)
	{
		MeshToMoveRotation.Yaw = RotationDirection * (Angle - 90.0f);

		if (RotationDirection > 0)
		{
			AnimationVariables.MovementDirection = EMovementDirection::Right;
		}
		else
		{
			AnimationVariables.MovementDirection = EMovementDirection::Left;
		}
	}
	else
	{
		AnimationVariables.MovementDirection = EMovementDirection::Backward;
		MeshToMoveRotation.Yaw = RotationDirection * (Angle - 180.0f);
	}

	return MeshToMoveRotation;
}

FRotator ATPPlayerCharacter::MeshToCameraInterpRotation(float Angle, float RotationDirection, float DeltaTime) const
{
	FRotator InterpolatedRotation = GetMesh()->GetComponentRotation();

	float TansitionSmoothness = (1.0f / RotationSmoothness) * PlayerRotationSpeed;
	float RotationAmount = TansitionSmoothness * RotationDirection * DeltaTime;

	if (FMath::Abs(RotationAmount) >= Angle)
	{
		InterpolatedRotation.Yaw = Angle * RotationDirection;
	}
	else
	{
		InterpolatedRotation.Yaw = RotationAmount;
	}

	return InterpolatedRotation;
}

void ATPPlayerCharacter::HandleAxeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this)
	{
		return;
	}

	ATPEnemyBase* Enemy = Cast<ATPEnemyBase>(OtherActor);
	if (Enemy == nullptr)
	{
		return;
	}

	if (OverlappedComponent == WeaponLeftBoxComponent)
	{
		LeftAxeOverlapEnemy = Enemy;
		IsLeftAxeOverlap = true;
	}
	else if(OverlappedComponent == WeaponRightBoxComponent)
	{
		RightAxeOverlapEnemy = Enemy;
		IsRightAxeOverlap = true;
	}
}

void ATPPlayerCharacter::HandleAxeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == this)
	{
		return;
	}

	ATPEnemyBase* Enemy = Cast<ATPEnemyBase>(OtherActor);
	if (Enemy == nullptr)
	{
		return;
	}

	if (OverlappedComponent == WeaponLeftBoxComponent)
	{
		LeftAxeOverlapEnemy = nullptr;
		IsLeftAxeOverlap = false;
	}
	else if (OverlappedComponent == WeaponRightBoxComponent)
	{
		RightAxeOverlapEnemy = nullptr;
		IsRightAxeOverlap = false;
	}
}

void ATPPlayerCharacter::CheckForEnemyHit(bool IsLeftHit)
{
	if (IsLeftHit)
	{
		if (!IsLeftAxeOverlap)
		{
			return;
		}

		PauseMontageOnHit();
		LeftAxeOverlapEnemy->GetHitFromSide(GetMeshForwardVector());
		UGameplayStatics::ApplyDamage(LeftAxeOverlapEnemy, WeaponDamage, GetController(), this, DamageType);
		if (SuccessfulHitFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SuccessfulHitFX, WeaponLeftBoxComponent->GetComponentLocation());
		}
	}
	else
	{
		if (!IsRightAxeOverlap)
		{
			return;
		}

		PauseMontageOnHit();
		RightAxeOverlapEnemy->GetHitFromSide(GetMeshForwardVector());
		UGameplayStatics::ApplyDamage(RightAxeOverlapEnemy, WeaponDamage, GetController(), this, DamageType);
		if (SuccessfulHitFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SuccessfulHitFX, WeaponRightBoxComponent->GetComponentLocation());
		}
	}
}

void ATPPlayerCharacter::PauseMontageOnHit()
{
	AnimInstance->Montage_SetPlayRate(AttackMontage, HitReducedPlayRate);

	GetWorldTimerManager().SetTimer(HitDelayTimerHandle, this, &ATPPlayerCharacter::UnpauseMontageAfterHit, HitAnimationDelay, false);
}

void ATPPlayerCharacter::UnpauseMontageAfterHit()
{
	AnimInstance->Montage_SetPlayRate(AttackMontage, 1.0f);
}

void ATPPlayerCharacter::BindAxeDelegates()
{
	// For some reason unreal autobinds the EndOverlap delegate so we have to clear in order to bind properly.
	// It also ignores the bindings if they happen in ctor.

	WeaponRightBoxComponent->OnComponentBeginOverlap.Clear();
	WeaponRightBoxComponent->OnComponentEndOverlap.Clear();
	WeaponRightBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ATPPlayerCharacter::HandleAxeBeginOverlap);
	WeaponRightBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ATPPlayerCharacter::HandleAxeEndOverlap);

	WeaponLeftBoxComponent->OnComponentBeginOverlap.Clear();
	WeaponLeftBoxComponent->OnComponentEndOverlap.Clear();
	WeaponLeftBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ATPPlayerCharacter::HandleAxeBeginOverlap);
	WeaponLeftBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ATPPlayerCharacter::HandleAxeEndOverlap);
}

void ATPPlayerCharacter::SetCurrentEnemyBoss(ATPEnemyBase* Boss)
{
	CurrentSelectedEnemy = Boss;
}

void ATPPlayerCharacter::SetCombatCameraEnabled(bool IsEnabled)
{
	IsCombatCameraEnabled = IsEnabled;
}

void ATPPlayerCharacter::AdjustTimers(float DeltaTime)
{
	if (InputMovementAxisX + InputMovementAxisY != 0.0f)
	{
		MovementInactivityTimer = 0.0f;
	}
	else
	{
		MovementInactivityTimer += DeltaTime;
	}
	
	if (InputCameraAxisPitch + InputCameraAxisYaw + InputControllerCameraAxisPitch + InputControllerCameraAxisYaw != 0.0f)
	{
		CameraInactivityTimer = 0.0f;
	}
	else
	{
		CameraInactivityTimer += DeltaTime;
	}
}

void ATPPlayerCharacter::CheckSetCameraAutoCenterEnabled()
{
	IsCameraAutoCenterEnabled = false;

	if (MovementInactivityTimer > CameraAutoCenterInactivityTime && CameraInactivityTimer > CameraAutoCenterInactivityTime)
	{
		IsCameraAutoCenterEnabled = true;
	}

	if (IsCombatCameraEnabled && CurrentSelectedEnemy != nullptr)
	{
		if (CameraInactivityTimer > 0.3f)
		{
			IsCameraAutoCenterEnabled = true;
		}
	}
}

void ATPPlayerCharacter::CheckActiveEnemyKilled(ATPEnemyBase* KilledEnemy)
{
	if (KilledEnemy == CurrentSelectedEnemy)
	{
		CurrentSelectedEnemy = nullptr;
	}
}

// TODO
// Camera Rotation Extends Smoothness
// Player Rotation Accel/Decel/Smoothness?
// Interpolate the transitions in mesh rotation
// Interpolate the forward vector in GetMeshForwardVector()? (May be sufficient to smooth out the player rotation)
