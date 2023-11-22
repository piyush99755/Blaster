// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};
UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AWeapon();

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	
	virtual void Tick(float DeltaTime) override;

	//function to determine whether or not display pickup widget..
	void ShowPickupWidget(bool bShowWidget);

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* WeaponSphere;

	UPROPERTY(VisibleAnywhere)
		EWeaponState WeaponState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* PickupWidget;



};
