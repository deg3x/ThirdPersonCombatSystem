// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPPHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UTPPHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(TP), meta=(BlueprintSpawnableComponent) )
class THIRDPERSON_API UTPPHealthComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Health")
	float MaxHealth = 100.0f;
	
	float CurrentHealth;

public:
	UPROPERTY(BlueprintAssignable, Category = "HealthEvents")
	FOnHealthChangedSignature OnHealthChanged;

public:	
	UTPPHealthComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	FORCEINLINE float GetMaxHealth()
	{
		return MaxHealth;
	}

	FORCEINLINE float GetCurrentHealth()
	{
		return CurrentHealth;
	}
};
