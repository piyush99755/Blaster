// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class BLASTER_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APickup();

	UPROPERTY(EditAnywhere)
		float BaseTurnRate = 45.f;

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FTimerHandle BindOverlapTimer;

	float BindOverlapTime = 0.25f;

	void BindOverlapTimeStart();

public:	
	
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override; 

private:

	UPROPERTY(EditAnywhere)
	class USphereComponent* OverlapSphere;


	UPROPERTY(EditAnywhere)
	class USoundCue* PickupSound;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere)
		class UNiagaraComponent* PickupEffectComponent;

	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* PickupEffect;

};
