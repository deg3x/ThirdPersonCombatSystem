// Fill out your copyright notice in the Description page of Project Settings.

#include "TPEnemyGrux.h"
#include "GameFramework\CharacterMovementComponent.h"

void ATPEnemyGrux::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate.Yaw = 260.0f;
}

void ATPEnemyGrux::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}