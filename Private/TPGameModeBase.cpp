// Fill out your copyright notice in the Description page of Project Settings.

#include "TPGameModeBase.h"
#include "TPPlayerCharacter.h"
#include "TPEnemyBase.h"
#include "TPHUD.h"

ATPGameModeBase::ATPGameModeBase()
{
	DefaultPawnClass = ATPPlayerCharacter::StaticClass();
	PlayerControllerClass = APlayerController::StaticClass();

	HUDClass = ATPHUD::StaticClass();
}

void ATPGameModeBase::InitiateBossFight(ATPEnemyBase* Enemy)
{
	Enemy->FinishWaitingForPlayer();
}