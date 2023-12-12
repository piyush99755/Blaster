// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UCombatComponent();

	friend class ABlasterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(class AWeapon* WeaponToEquip);

	void InterpFOV(float DeltaTime);

	

	
		

protected:
	
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);
	//server RPC function for client side 
    UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireButtonPressed(bool bPressed);

	//server RPC function to handle fire functionality on server
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

   //multicast RPC function to handle fire functionality on both server and client
	UFUNCTION(NetMulticast, Reliable)
		void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& HitResult);

	void SetHUDCrosshairs(float DeltaTime);

private:

	
	class ABlasterCharacter* Character;

	class ABlasterPlayerController* Controller;

	class ABlasterHUD* HUD;

	//make it Replicated to replicate EquippedWeapon variable
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	UPROPERTY(EditAnywhere)
	bool bFireButtonPressed;

	float DefaultFOV; 

	float CurrentFOV;
	
	UPROPERTY(EditAnywhere)
		float ZoomFOV = 30.f;

	UPROPERTY(EditAnywhere)
		float ZoomInterpSpeed = 20.f;
	
	float CrosshairVelocityFactor;

	float CrosshairInAirFactor; 

	float CrosshairAimFactor;

	float CrosshairShootingFactor; 

	FVector HitTarget;

	FHUDPackage HUDPackage;
	

	
	
	

		
};
