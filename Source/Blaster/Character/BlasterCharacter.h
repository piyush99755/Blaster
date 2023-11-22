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
	
	//special type of Uproperty used to make variable replicable..
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon;

	//rep notify
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public:

	
	void SetOverlappingWeapon(AWeapon* Weapon);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void Lookup(float Value);

};
