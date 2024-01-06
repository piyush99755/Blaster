// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"



URocketMovementComponent::EHandleBlockingHitResult URocketMovementComponent::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
	Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);
	return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void URocketMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	//ROCKETS sHOULD NOT STOP. ONLY EXPLODE WHEN COLLISION BOX REGISTER HIT EVENTS ..
}
