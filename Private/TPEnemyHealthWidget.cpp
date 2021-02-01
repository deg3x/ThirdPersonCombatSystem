// Fill out your copyright notice in the Description page of Project Settings.

#include "TPEnemyHealthWidget.h"

#include "TPEnemyBase.h"

void UTPEnemyHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTPEnemyHealthWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (TargetHealth != MaxHealth)
	{
		TimeSinceLastUpdate += InDeltaTime;
	}

	UpdateUI(InDeltaTime);
	UpdateFillPercentages();
}

void UTPEnemyHealthWidget::UpdateUI(float DeltaTime)
{
	if (TargetHealth == CurrentHealthBack && TargetHealth == CurrentHealthFront)
	{
		// TARGET DUMMY RESET HEALTH
		if (CurrentHealthBack == 0.0f && TimeSinceLastUpdate > TimeToUpdate)
		{
			EnemyOwner->FullyHeal();
		}

		TimeInterpolatingBack = 0.0f;
		TimeInterpolatingFront = 0.0f;

		return;
	}

	if (TargetHealth != CurrentHealthFront)
	{
		float min = CachedHealthFront > TargetHealth ? TargetHealth : CachedHealthFront;
		float max = CachedHealthFront > TargetHealth ? CachedHealthFront : TargetHealth;
		float alpha = CachedHealthFront > TargetHealth ? 1.0f - TimeInterpolatingFront : TimeInterpolatingFront;
		CurrentHealthFront = FMath::InterpEaseInOut(min, max, alpha, 3);
		CurrentHealthFront = FMath::Clamp(CurrentHealthFront, min, max);
		TimeInterpolatingFront += DeltaTime / TimeToFillFront;
	}
	else
	{
		TimeInterpolatingFront = 0.0f;
	}

	if (TimeSinceLastUpdate < TimeToUpdate && CachedHealthBack > TargetHealth)
	{
		TimeInterpolatingBack = 0.0f;

		return;
	}

	float min = CachedHealthBack > TargetHealth ? TargetHealth : CachedHealthBack;
	float max = CachedHealthBack > TargetHealth ? CachedHealthBack : TargetHealth;
	float alpha = CachedHealthBack > TargetHealth ? 1.0f - TimeInterpolatingBack : TimeInterpolatingBack;
	CurrentHealthBack = FMath::InterpEaseInOut(min, max, alpha, 3);
	CurrentHealthBack = FMath::Clamp(CurrentHealthBack, min, max);
	
	TimeInterpolatingBack += DeltaTime / TimeToFillBack;
}

void UTPEnemyHealthWidget::UpdateFillPercentages()
{
	FillAmountFront = CurrentHealthFront / MaxHealth;
	FillAmountBack = CurrentHealthBack / MaxHealth;
}