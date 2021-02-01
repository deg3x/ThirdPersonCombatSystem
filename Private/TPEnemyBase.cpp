// Fill out your copyright notice in the Description page of Project Settings.

#include "TPEnemyBase.h"
#include "Components\SkeletalMeshComponent.h"
#include "Animation\AnimInstance.h"
#include "AIController.h"
#include "TPPlayercharacter.h"
#include "TPEnemyAIController.h"
#include "TPEnemyHealthWidget.h"
#include "TPWidgetToCameraComponent.h"
#include "TPPHealthComponent.h"
#include "Components\CapsuleComponent.h"
#include "Kismet\GameplayStatics.h"
#include "TimerManager.h"
#include "BrainComponent.h"

ATPEnemyBase::ATPEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthWidgetComponent = CreateDefaultSubobject<UTPWidgetToCameraComponent>(TEXT("HealthWidget"));
	HealthWidgetComponent->SetTickWhenOffscreen(true);
	HealthWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 1.3f * GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	HealthComponent = CreateDefaultSubobject<UTPPHealthComponent>(TEXT("HealthComponent"));
}

void ATPEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	AnimationVariables.IsDead = false;
	AnimInstance = GetMesh()->GetAnimInstance();
	HealthComponent->OnHealthChanged.AddDynamic(this, &ATPEnemyBase::OnHealthChanged);

	InitHealthUI();
	HideHealthUI();
	PlayWaitForPlayerMontage();
}

void ATPEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATPEnemyBase::PlayWaitForPlayerMontage()
{
	float SectionStart;
	float SectionEnd;
	WaitPlayerMontage->GetSectionStartAndEndTime(1, SectionStart, SectionEnd);
	AnimInstance->Montage_Play(WaitPlayerMontage, 1.0f, EMontagePlayReturnType::Duration, SectionStart);
	AnimInstance->Montage_Pause();
}

void ATPEnemyBase::FinishWaitingForPlayer()
{
	AnimInstance->Montage_Resume(WaitPlayerMontage);

	ShowHealthUI();

	if (AnimInstance->GetCurrentActiveMontage() != WaitPlayerMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[!] Invalid active montage"));

		return;
	}

	FAnimMontageInstance* RunningMontage = AnimInstance->GetActiveMontageInstance();

	if (RunningMontage == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[!] Invalid active montage instance"));

		return;
	}

	float TimeLeft = WaitPlayerMontage->GetSectionTimeLeftFromPos(RunningMontage->GetPosition());

	if (TimeLeft == -1.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[!] Incorrect running montage position"));

		return;
	}

	// GetWorldTimerManager().SetTimer(IntroTimerHandle, this, &ATPEnemyBase::StartBehaviorTree, TimeLeft, false); AI NOT READY
}

void ATPEnemyBase::GetHitFromSide(FVector PlayerForward)
{
	FVector EnemyForward = GetActorForwardVector();
	FVector PlayerForwardNeg = -PlayerForward;

	float Angle = FMath::Acos(FVector::DotProduct(EnemyForward, PlayerForwardNeg)) * 180.0f / PI;
	float Direction = FVector::CrossProduct(EnemyForward, PlayerForwardNeg).Z > 0.0f ? 1.0f : -1.0f;

	int32 SectionID;

	if (Angle < 40.0f)
	{
		SectionID = TakeHitMontage->GetSectionIndex(TEXT("HitFront"));
	}
	else if (Angle < 140.0f)
	{
		if (Direction == -1.0f)
		{
			SectionID = TakeHitMontage->GetSectionIndex(TEXT("HitLeft"));
		}
		else
		{
			SectionID = TakeHitMontage->GetSectionIndex(TEXT("HitRight"));
		}
	}
	else
	{
		SectionID = TakeHitMontage->GetSectionIndex(TEXT("HitBack"));
	}

	PlayTakeHitSection(SectionID);
}

void ATPEnemyBase::PlayTakeHitSection(int32 SectionID)
{
	float StartTime;
	float EndTime;
	TakeHitMontage->GetSectionStartAndEndTime(SectionID, StartTime, EndTime);
	AnimInstance->Montage_Play(TakeHitMontage, 1.0f, EMontagePlayReturnType::Duration, StartTime);
}

void ATPEnemyBase::ShowHealthUI()
{
	HealthWidgetComponent->SetVisibility(true);
}

void ATPEnemyBase::HideHealthUI()
{
	HealthWidgetComponent->SetVisibility(false);
}

void ATPEnemyBase::StartBehaviorTree()
{
	ATPEnemyAIController* AIController = GetController<ATPEnemyAIController>();
	if (AIController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Controller not found!"));
	}

	AIController->ExecuteBehaviorTree();
}

void ATPEnemyBase::StopBehaviorTree(FString Reason)
{
	ATPEnemyAIController* AIController = GetController<ATPEnemyAIController>();
	if (AIController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Controller not found!"));
	}

	AIController->BrainComponent->StopLogic(Reason);
}

void ATPEnemyBase::OnHealthChanged(UTPPHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UpdateHealthUI(Health);

	/* NO NEED TO DIE YET. USE AS A COMBAT DUMMY FOR NOW
	if (Health <= 0.0f)
	{
		AnimationVariables.IsDead = true;

		StopBehaviorTree(FString(TEXT("Died"))); AI NOT READY
		HideHealthUI();
		GetCapsuleComponent()->Deactivate();
		
		ATPPlayerCharacter* Player = Cast<ATPPlayerCharacter>(DamageCauser);
		if (Player != nullptr)
		{
			Player->CheckActiveEnemyKilled(this);
		}

		SetLifeSpan(3.0f);
	}*/
}

void ATPEnemyBase::InitHealthUI()
{
	HealthWidgetComponent->SetHealthWidgetValues(HealthComponent->GetMaxHealth(), HealthComponent->GetMaxHealth());
	HealthWidgetComponent->SetEnemyOwner(this);
}

void ATPEnemyBase::UpdateHealthUI(float CurrentHealth)
{
	HealthWidgetComponent->UpdateHealthWidgetCurrentHealth(CurrentHealth);
}

void ATPEnemyBase::FullyHeal()
{
	TSubclassOf<UDamageType> HealDamage;
	UGameplayStatics::ApplyDamage(this, -HealthComponent->GetMaxHealth(), GetController(), this, HealDamage);
}