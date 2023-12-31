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
		class UAnimMontage* WeaponFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* DeathMontage;

	//Player Health variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere)
		float Health = 100.f;

	UPROPERTY()
		class ABlasterPlayerController* BlasterPlayerController;




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

	void HideCameraIfClose();



	void SimulatedProxiesTurn();

	//inherited function from character class 

	virtual void OnRep_ReplicatedMovement() override;

	float CalculateSpeed();

	//health variable and function..
	UFUNCTION()
		void OnRep_Health();

	void UpdateHUDHealth();

	//variables and function for eliminate player on death 
	void Elimination();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastElimination();



	FTimerHandle ElimTimerHandle;

	UPROPERTY(EditAnywhere)
		float ElimDelay = 1.5f;

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



	bool IsAiming();

	AWeapon* GetEquippedWeapon();

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsEliminated() const { return bEliminated; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	

	ECombatState GetCombatState() const;

	FVector GetHitTarget() const;

	

	

};
