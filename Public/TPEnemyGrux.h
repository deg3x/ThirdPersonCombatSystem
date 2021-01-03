// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPEnemyBase.h"
#include "TPEnemyGrux.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API ATPEnemyGrux : public ATPEnemyBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
