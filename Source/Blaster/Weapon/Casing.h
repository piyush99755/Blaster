// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class BLASTER_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ACasing();

	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


public:	

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* CasingMesh;

	UPROPERTY(EditAnywhere)
		float ShellImpulse;

	UPROPERTY(EditAnywhere)
		class USoundCue* ShellSound;
	
	

};
