// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairInterface.h"
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
		 UAnimMontage* HitReactMontage;

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



protected:
	
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	virtual void Jump() override;

	void FireButtonPressed();

	void FireButtonReleased();

public:	
	
	virtual void Tick(float DeltaTime) override;

	void AimOffset(float DeltaTime);

	void TurnInPlace(float DeltaTime);

	void PlayFireWeaponMontage(bool bAiming);

	void PlayHitReactMontage();

	void HideCameraIfClose();

	//multicast RPC to reflect on client and server
	UFUNCTION(NetMulticast, Unreliable)
		void MulticastHit();
	
	


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

	

	bool IsAiming();

	AWeapon* GetEquippedWeapon();

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch;  }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FVector GetHitTarget() const;

	

	

};
