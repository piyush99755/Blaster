// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	void SetHealthHUD(float Health, float MaxHealth);

	//void SetScoreHUD(float Score);

	void SetWeaponAmmoHUD(int32 Ammo);

	void SetCarriedAmmoHUD(int32 Ammo);

	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	UPROPERTY()
	class ABlasterCharacter* BlasterCharacter;

protected:

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn)override;
	
};
