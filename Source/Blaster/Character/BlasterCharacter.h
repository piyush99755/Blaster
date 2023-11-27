// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
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



protected:
	
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

public:	
	
	virtual void Tick(float DeltaTime) override;

	
	


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

	

};
