// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "TPWidgetToCameraComponent.generated.h"

class UTPEnemyHealthWidget;
class ATPEnemyBase;

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UTPWidgetToCameraComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	UTPEnemyHealthWidget* HealthWidget;

public:
	UTPWidgetToCameraComponent();

protected:
	virtual void BeginPlay() override;

	void FacePlayerCamera();

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UpdateHealthWidgetCurrentHealth(float CurrentHealth);
	void SetHealthWidgetValues(float MaxHealth, float CurrentHealth);
	void SetEnemyOwner(ATPEnemyBase* Owner);
};
