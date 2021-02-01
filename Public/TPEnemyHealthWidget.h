// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TPEnemyHealthWidget.generated.h"

class ATPEnemyBase;

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UTPEnemyHealthWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI Variables")
	float FillAmountFront = 1.0f;
	UPROPERTY(BlueprintReadOnly, Category = "UI Variables")
	float FillAmountBack = 1.0f;
	UPROPERTY(EditAnywhere, Category = "UI Variables")
	float TimeToUpdate = 1.2f;
	UPROPERTY(EditAnywhere, Category = "UI Variables")
	float TimeToFillFront = 0.2f;
	UPROPERTY(EditAnywhere, Category = "UI Variables")
	float TimeToFillBack = 1.0f;

	float MaxHealth;
	float CachedHealthFront;
	float CachedHealthBack;
	float CurrentHealthFront;
	float CurrentHealthBack;
	float TargetHealth;
	float TimeSinceLastUpdate;
	float TimeInterpolatingFront;
	float TimeInterpolatingBack;
	ATPEnemyBase* EnemyOwner;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateUI(float DeltaTime);
	void UpdateFillPercentages();

public:
	FORCEINLINE void UpdateCurrentHealth(float NewCurrentHealth)
	{
		CachedHealthFront = CurrentHealthFront;
		CachedHealthBack = CurrentHealthBack;
		TargetHealth = NewCurrentHealth;
		TimeSinceLastUpdate = 0.0f;
	}

	FORCEINLINE void SetMaxHealth(float NewMaxHealth)
	{
		MaxHealth = NewMaxHealth;
	}

	FORCEINLINE void SetCurrentHealth(float NewCurrentHealth)
	{
		CurrentHealthFront = NewCurrentHealth;
		CurrentHealthBack = NewCurrentHealth;
		TargetHealth = NewCurrentHealth;
	}

	FORCEINLINE void SetEnemyOwner(ATPEnemyBase* Owner)
	{
		EnemyOwner = Owner;
	}
};
