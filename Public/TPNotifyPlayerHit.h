// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "TPNotifyPlayerHit.generated.h"

UENUM(BlueprintType)
enum class EHitSide : uint8
{
	Left,
	Right
};

UCLASS()
class THIRDPERSON_API UTPNotifyPlayerHit : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Animation Notify")
	EHitSide HitSide;

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
