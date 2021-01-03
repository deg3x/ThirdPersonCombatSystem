// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TPEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API ATPEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Behavior")
	UBehaviorTree* BehaviorTree;

public:
	void ExecuteBehaviorTree();
	void InitializeBlackboardData();
};
