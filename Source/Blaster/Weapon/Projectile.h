// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovementComponent;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SpawnTrailSystem();

	void ExplodeDamage();

protected:
	
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

private:
	

	

	//variables for projectile tracer, while its travelling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* Tracer;

	
	class UParticleSystemComponent* TracerComponent;

	

public:	


	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* CollisionBox;

	//variables for projectile hit impact particles and sound 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* TrailSystem;

	UPROPERTY()
		class UNiagaraComponent* TrailSystemComponent;


	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
		float DestroyTime = 3.f;

	void StartDestroyTimer();

	void DestroyTimeFinished();

	
	
	

};
