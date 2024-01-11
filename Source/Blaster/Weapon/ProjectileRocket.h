// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

		AProjectileRocket();
public:


    UPROPERTY(EditAnywhere)
		class URocketMovementComponent* RocketMovementComponent;

	
	UPROPERTY(EditAnywhere)
		class USoundCue* ProjectileLoopSound;

	UPROPERTY()
	class UAudioComponent* ProjectileLoopComponent;

	UPROPERTY(EditAnywhere)
		class USoundAttenuation* ProjectileLoopAttenuation;


	

	//overrding destroyed function from base class..

	virtual void Destroyed();

protected:

	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	virtual void BeginPlay();
	
};
