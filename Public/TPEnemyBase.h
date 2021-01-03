// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPEnemyBase.generated.h"

USTRUCT(BlueprintType)
struct FEnemyAnimProperties
{
	GENERATED_BODY()

	bool IsMoving;
};

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

	UAnimInstance* AnimInstance;

public:
	ATPEnemyBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void PlayWaitForPlayerMontage();
	void FinishWaitingForPlayer();
	void GetHitFromSide(FVector PlayerForward);
	void PlayTakeHitSection(int32 SectionID);
};
