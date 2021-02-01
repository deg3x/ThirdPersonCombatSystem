// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPEnemyBase.generated.h"

USTRUCT(BlueprintType)
struct FEnemyAnimProperties
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Animation State")
	bool IsDead;
};

class UTPPHealthComponent;
class UTPWidgetToCameraComponent;

UCLASS()
class THIRDPERSON_API ATPEnemyBase : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Movement Variables")
	float MovementSpeed = 45.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Animation Variables")
	FEnemyAnimProperties AnimationVariables;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* WaitPlayerMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* TakeHitMontage;

	UPROPERTY(EditAnywhere, Category = "UI")
	UTPWidgetToCameraComponent* HealthWidgetComponent;

	UPROPERTY(Editanywhere, Category = "Stats")
	UTPPHealthComponent* HealthComponent;

	UAnimInstance* AnimInstance;
	FTimerHandle IntroTimerHandle;

public:
	ATPEnemyBase();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void StartBehaviorTree();

	void StopBehaviorTree(FString Reason);

	UFUNCTION()
	void OnHealthChanged(UTPPHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void PlayWaitForPlayerMontage();
	void FinishWaitingForPlayer();
	void GetHitFromSide(FVector PlayerForward);
	void PlayTakeHitSection(int32 SectionID);
	void ShowHealthUI();
	void HideHealthUI();
	void InitHealthUI();
	void UpdateHealthUI(float CurrentHealth);
	void FullyHeal();
};
