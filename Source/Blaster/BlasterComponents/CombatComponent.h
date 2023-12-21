// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatState.h"
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

	void Reload();

	void InterpFOV(float DeltaTime);

	//function to implmenet in animation blueprint so prevent playing montage spamming 
	UFUNCTION(BlueprintCallable)
	void FinishReloading();



	

	
		

protected:
	
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);
	//server RPC function for client side 
    UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireButtonPressed(bool bPressed);

	void Fire();

	//server RPC function to handle fire functionality on server
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

   //multicast RPC function to handle fire functionality on both server and client
	UFUNCTION(NetMulticast, Reliable)
		void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
		void ServerReload();

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

	bool CanFire();

	//zoom in variables..
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

	//HUD package
	FHUDPackage HUDPackage;

	//automatic fire variables..

	FTimerHandle FireHandle;

	

	bool bCanFire = true;

	void FireTimerStart();

	
	void FireTimeFinished();

	//variable and function for carried ammo
	//carried ammo for currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo; 

	UFUNCTION()
	void OnRep_CarriedAmmo();

	//using TMAP as different type of weapons will be used and ammo for different types of weapon will be different amount
	TMap<EWeaponType, int32> CarriedAmmoMap; //key, value pair... 

	int32 StartingARAmmo = 100;

	void InitializeCarriedAmmo();

	//reloading
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void HandleReload();



	

	
	
	

		
};
