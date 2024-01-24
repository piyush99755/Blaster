// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPickupPoint.generated.h"

UCLASS()
class BLASTER_API ASpawnPickupPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ASpawnPickupPoint();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickup>> PickupClasses;

	APickup* SpawnedPickup;

	void SpawnPickup();

	FTimerHandle SpawnPickupTimer;

	//it is a callback function to bind to the OnDestroyed() Multicast delegate
	UFUNCTION()
	void SpawnPickupTimerStart(AActor* DestroyedActor);

	void SpawnPickupTimerFinished();


private:
    
	UPROPERTY(EditAnywhere)
	float SpawnPickupMinTime;

	UPROPERTY(EditAnywhere)
	float SpawnPickupMaxTime;



public:	
	
	virtual void Tick(float DeltaTime) override;

};
