// Fill out your copyright notice in the Description page of Project Settings.

#include "TPNotifyPlayerHit.h"
#include "Components/SkeletalMeshComponent.h"
#include "TPPlayerCharacter.h"

void UTPNotifyPlayerHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ATPPlayerCharacter* Player = Cast<ATPPlayerCharacter>(MeshComp->GetOwner());

	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[!] Could not properly get player in Hit animation notify"));
		return;
	}

	if (HitSide == EHitSide::Left)
	{
		Player->CheckForEnemyHit(true);
	}
	else
	{
		Player->CheckForEnemyHit(false);
	}
}