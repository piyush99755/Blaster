// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AHealthPickup : public APickup
{
	GENERATED_BODY()

		AHealthPickup();

private:

	UPROPERTY(EditAnywhere)
		float HealingTime = 5.f;

	UPROPERTY(EditAnywhere)
		float HealingHealth = 100.f;

	UPROPERTY(VisibleAnywhere)
		class UNiagaraComponent* PickupEffectComponent;

	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* PickupEffect;




protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:

	virtual void Destroyed() override;
};
