// Fill out your copyright notice in the Description page of Project Settings.

#include "TPEnemyBase.h"
#include "Components\SkeletalMeshComponent.h"
#include "Animation\AnimInstance.h"
#include "AIController.h"
#include "TPEnemyAIController.h"

ATPEnemyBase::ATPEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ATPEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	AnimInstance = GetMesh()->GetAnimInstance();
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
	ATPEnemyAIController* AIController = GetController<ATPEnemyAIController>();
	if (AIController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller not found!"));
	}

	AIController->ExecuteBehaviorTree();
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