// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPCombatAreaTrigger.generated.h"

class UBoxComponent;
class ATPEnemyBase;

UCLASS()
class THIRDPERSON_API ATPCombatAreaTrigger : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Components")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditInstanceOnly, Category = "Bindings")
	ATPEnemyBase* BoundEnemy;

public:	
	ATPCombatAreaTrigger();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
