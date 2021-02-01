// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TPHUD.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API ATPHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATPHUD();

	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;
};
