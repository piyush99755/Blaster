// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPickupPoint.h"
#include "Pickup.h"


ASpawnPickupPoint::ASpawnPickupPoint()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

void ASpawnPickupPoint::BeginPlay()
{
	Super::BeginPlay();

	SpawnPickupTimerStart((AActor*)nullptr);
	
}

void ASpawnPickupPoint::SpawnPickup()
{
	int32 NumOfPickups = PickupClasses.Num();
	if (NumOfPickups > 0)
	{
		//spawn pickup from random range between 0 and num of pickups
		int32 Selection = FMath::RandRange(0, NumOfPickups - 1);
		SpawnedPickup = GetWorld()->SpawnActor<APickup>(PickupClasses[Selection], GetActorTransform());

		//when spawned pickup has been destroyed , it will call SpawnPickupTimerStart callback function to spawn new pickup
		if (HasAuthority() && SpawnedPickup)
		{
			SpawnedPickup->OnDestroyed.AddDynamic(this, &ASpawnPickupPoint::SpawnPickupTimerStart);
		}
	}
}


void ASpawnPickupPoint::SpawnPickupTimerStart(AActor* DestroyedActor)
{
	const float SpawnTime = FMath::FRandRange(SpawnPickupMinTime, SpawnPickupMaxTime);

	//adding delay before spawn pickup timer start
	GetWorldTimerManager().SetTimer(SpawnPickupTimer, this, &ASpawnPickupPoint::SpawnPickupTimerFinished, SpawnTime);
}

void ASpawnPickupPoint::SpawnPickupTimerFinished()
{
	if (HasAuthority())
	{
		SpawnPickup();
	}
}

void ASpawnPickupPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

