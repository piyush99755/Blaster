// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "JumpPIckup.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AJumpPIckup : public APickup
{
	GENERATED_BODY()


protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	UPROPERTY(EditAnywhere)
	float BuffJumpVelocity = 4000.f;

	UPROPERTY(EditAnywhere)
		float BuffJumpTime = 30.f;
	
};
