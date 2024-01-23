// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UBuffComponent();

	friend class ABlasterCharacter;



protected:
	
	virtual void BeginPlay() override;

private:

	UPROPERTY()
	class ABlasterCharacter* Character;

	//heal buff variables
	bool bHealing = false;

	float HealingRate = 0.f;

	float AmountToHeal = 0.f;

	//speed buff variables

	FTimerHandle SpeedBuffTimer;

	void ResetSpeeds();

	float SpeedBuffTime = 30.f;

	float InitialBaseSpeed;

	float InitialCrouchSpeed; 

	//jump buff variable

	FTimerHandle JumpBuffTimer;

	void ResetJump();

	float InitialJumpVelocity;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Heal(float HealingAmount, float HealingTime);

	void HealRampUp(float DeltaTime);

	void BuffSpeed(float BaseSpeedBuff, float CrouchSpeedBuff, float BuffTime);

	void BuffJump(float BuffJumpVelocity, float BuffTime);

	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);

	void SetInitialJumpVelocity(float JumpVelocity);

	//multicast RPC to takecare of client side and syncing server and client with each other 
	UFUNCTION(NetMulticast, Reliable)
	void MulticastBuffSpeed(float BaseSpeed, float CrouchSpeed);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastBuffJump(float JumpVelocity);
};
