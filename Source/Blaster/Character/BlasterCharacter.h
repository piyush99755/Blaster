// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairInterface.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent* OverheadWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UCombatComponent* CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBuffComponent* BuffComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class ULagCompensationComponent* LagCompensation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* AttachedGrenade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* WeaponFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* ThrowGrenadeMontage;

	//Player Health variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere)
		float Health = 100.f;

	UPROPERTY()
		class ABlasterPlayerController* BlasterPlayerController;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AWeapon> DefaultWeaponClass;

	//Hit boxes for server-side rewind..

	UPROPERTY(EditAnywhere)
	class UBoxComponent* head;

	UPROPERTY(EditAnywhere)
	 UBoxComponent* Pelvis;


	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent* UpperArm_L;

	UPROPERTY(EditAnywhere)
	UBoxComponent* UpperArm_R;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Hand_L;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Hand_R;

	UPROPERTY(EditAnywhere)
	UBoxComponent* backpack;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Thigh_L;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Thigh_R;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Foot_L;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Foot_R;

	UPROPERTY()
		TMap<FName, class UBoxComponent*> HitBoxCollisions;


private:
	//special type of Uproperty used to make variable replicable..
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon;

	//rep notify
	UFUNCTION()
		void OnRep_OverlappingWeapon(class AWeapon* LastWeapon);



	//create RPC(Remote Procedure Calls)function in intention to call this in client and executed on server
	UFUNCTION(Server, Reliable)
		void ServerEquipButtonPressed();

	float AO_Yaw;

	float InterpAO_Yaw;

	float AO_Pitch;

	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	float CameraThreshold = 100.f;

	//variables to implement smooth rotation for simulated proxies..
	bool bRotateRootBone;

	float TurnThreshold = 0.5f;

	FRotator SimulatedProxyRotationLastFrame;

	FRotator SimulatedProxyRotation;

	float SimulatedProxyYaw;

	float TimeSinceLastReplicatedMovement;

	bool bEliminated = false;

	






protected:

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	virtual void Jump() override;

	void FireButtonPressed();

	void FireButtonReleased();

	UFUNCTION()
		void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void SpawnDefaultWeapon();

public:

	virtual void Tick(float DeltaTime) override;

	void AimOffset(float DeltaTime);

	void CalculateAO_Pitch();

	void RotateInPlace(float DeltaTime);

	void TurnInPlace(float DeltaTime);

	void PlayFireWeaponMontage(bool bAiming);

	void PlayReloadMontage();

	void PlayHitReactMontage();

	void PlayDeathMontage();

	void PlayThrowGrenadeMontage();

	void HideCameraIfClose();



	void SimulatedProxiesTurn();

	//inherited function from character class 

	virtual void OnRep_ReplicatedMovement() override;

	float CalculateSpeed();

	//health variable and function..
	UFUNCTION()
		void OnRep_Health(float LastHealth);

	

	void UpdateHUDHealth();

	void UpdateAmmoHUD();

	

	//variables and function for eliminate player on death 
	void Elimination();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastElimination();

	void DropOrDestroyWeapons();

	void DropOrDestroyWeapon(AWeapon* Weapon);
	FTimerHandle ElimTimerHandle;

	UPROPERTY(EditAnywhere)
		float ElimDelay = 2.06f;

	void ElimTimerFinished();

	UFUNCTION(BlueprintCallable)
		void DeathFinished();

	UPROPERTY(Replicated)
		bool bDisableGameplay = false;

	




public:


	void SetOverlappingWeapon(class AWeapon* Weapon);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void Lookup(float Value);

	void EquipButtonPressed();

	//function to update bool value in anim instace..
	bool IsWeaponEquipped();

	void CrouchButtonPressed();

	void AimButtonPressed();

	void AimButtonReleased();

	void ReloadButtonPressed();

	void ThrowGrenadeButtonPressed();

	



	bool IsAiming();

	AWeapon* GetEquippedWeapon();

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsEliminated() const { return bEliminated; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE UStaticMeshComponent* GetGrenadeMesh() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* GetBuffComponent() const { return BuffComponent; }
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const  { return LagCompensation; }

	bool IsLocallyReloading();
	

	ECombatState GetCombatState() const;

	FVector GetHitTarget() const;

	UFUNCTION(BlueprintImplementableEvent)
		void ShowSniperScoreWidget(bool bShowScopeWidget);

	

	

};
