// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:

	virtual void Fire(const FVector& HitTarget)override;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
		 UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere)
		UParticleSystem* FireParticles;

	UPROPERTY(EditAnywhere)
		class USoundCue* FireSound;

	UPROPERTY(EditAnywhere)
		USoundCue* HitSound;

	//trace end with scatter variables.. 
	UPROPERTY(EditAnywhere)
		float DistaceToSphere = 800.f;

	UPROPERTY(EditAnywhere)
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere)
	bool bUseScatter = false;

protected:

	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);

	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);





	
};
