// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"



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

	void SwapWeapons();

	bool ShouldSwapWeapons();

	void ReloadEmptyWeapon();

	void PlayWeaponEquipSound(AWeapon* WeaponToEquip);

	void UpdateCarriedAmmo();

	void AttachActorToRightHand(AActor* ActorToAttach);

	void AttachActorToLeftHand(AActor* ActorToAttach);

	void AttachActorToBackpack(AActor* ActorToAttach);

	void DropEquipWeapon();

	void FireButtonPressed(bool bPressed);

	void Reload();

	void InterpFOV(float DeltaTime);

	//function to implmenet in animation blueprint so prevent playing montage spamming 
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	FORCEINLINE int32 GetGrenades() const { return Grenades; }

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);



	

	
		

protected:
	
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);
	//server RPC function for client side 
    UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
		void OnRep_EquippedSecondaryWeapon();

	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);

	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);

	

	void Fire();

	void FireHitScanWeapon();

	void FireProjectileWeapon();

	void LocalFire(const FVector_NetQuantize& TraceHitTarget);

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

	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	UFUNCTION(BlueprintCallable)
		void FinishThrowGrenade();

	UFUNCTION(BlueprintCallable)
		void LaunchGrenade();


	UFUNCTION(Server, Reliable)
		void ServerLaunchGrenade(const FVector_NetQuantize& Target);

	void ShowAttachedGrenade(bool bShowGrenade);

	

	


private:

	
	class ABlasterCharacter* Character;

	class ABlasterPlayerController* Controller;

	class ABlasterHUD* HUD;

	//make it Replicated to replicate EquippedWeapon variable
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeapon* EquippedWeapon;

	//secondary weapon 
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
		AWeapon* EquippedSecondaryWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;

	bool bAimingButtonPressed = false;

	UFUNCTION()
	void OnRep_Aiming();

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

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 100;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0.f;

	UPROPERTY(EditAnywhere)
		int32 StartingPistolAmmo = 15;

	UPROPERTY(EditAnywhere)
		int32 StartingSMGAmmo = 20;

	UPROPERTY(EditAnywhere)
		int32 StartingShotgunAmmo = 10;

	UPROPERTY(EditAnywhere)
		int32 StartingSniperRifleAmmo = 5;

	UPROPERTY(EditAnywhere)
		int32 StartingGrenadeLauncherAmmo = 5;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AProjectile> GrenadeClass;

	UPROPERTY(ReplicatedUsing = OnRep_Grenades)
	int32 Grenades = 5;

	UPROPERTY(EditAnywhere)
	int32 MaxGrenades = 20;

	UPROPERTY(EditAnywhere)
		int32 MaxCarriedAmmo = 999;

	UFUNCTION()
	void OnRep_Grenades();

	void UpdateGrenades();


	void InitializeCarriedAmmo();

	//reloading
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void HandleReload();
	 
	int32 AmountToReload();

	void UpdateAmmoValues();

	bool bLocallyReloading = false;



	

	
	
	

		
};
