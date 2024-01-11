// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileGrenade.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileGrenade : public AProjectile
{
	GENERATED_BODY()

		AProjectileGrenade();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	virtual void Destroyed() override;

private:
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta =(AllowPrivateAccess = "true"))
	class USoundCue* BounceSound; 
	
};
