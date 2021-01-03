// Fill out your copyright notice in the Description page of Project Settings.

#include "TPEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TPPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATPEnemyAIController::ExecuteBehaviorTree()
{
	RunBehaviorTree(BehaviorTree);
	InitializeBlackboardData();
}

void ATPEnemyAIController::InitializeBlackboardData()
{
	ATPPlayerCharacter* Player = Cast<ATPPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player object not found while initializing blackboard data"))
		return;
	}
	
	Blackboard->SetValueAsObject(TEXT("Player"), Player);
}