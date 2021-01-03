// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPGameModeBase.generated.h"

class ATPEnemyBase;

/**
 * 
 */
UCLASS()
class THIRDPERSON_API ATPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATPGameModeBase();

	void InitiateBossFight(ATPEnemyBase* Enemy);
};
