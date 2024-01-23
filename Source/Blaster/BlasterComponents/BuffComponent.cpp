// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"


UBuffComponent::UBuffComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;


}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}






void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);


}

void UBuffComponent::Heal(float HealingAmount, float HealingTime)
{
	//calculating variable values to implement heal functionality 
	bHealing = true;
	HealingRate = HealingAmount / HealingTime;
	AmountToHeal += HealingAmount; 
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsEliminated()) return;

	//calculating heal at current frame and then clamp its value between 0 and max health..
	const float HealThisFrame = HealingRate * DeltaTime; 

	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));

	//this willtake care of server side
	Character->UpdateHUDHealth();

	AmountToHeal -= HealThisFrame;

	//when character is healed enough, it stops healing
	if (AmountToHeal <= 0.f ||  Character->GetHealth() >= Character->GetMaxHealth())
    {
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void UBuffComponent::BuffSpeed(float BaseSpeedBuff, float CrouchSpeedBuff, float BuffTime)
{
	if (Character == nullptr ) return;

	//using timer handle to reset character speeds back to normal after time run out
	Character->GetWorldTimerManager().SetTimer(SpeedBuffTimer, this, &UBuffComponent::ResetSpeeds, BuffTime);

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeedBuff;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeedBuff;
	}

	MulticastBuffSpeed(BaseSpeedBuff, CrouchSpeedBuff);
}



void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;

	InitialCrouchSpeed = CrouchSpeed;
}





void UBuffComponent::ResetSpeeds()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;

	MulticastBuffSpeed(InitialBaseSpeed, InitialCrouchSpeed);
	
}

void UBuffComponent::MulticastBuffSpeed_Implementation(float BaseSpeed, float CrouchSpeed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;

	
	
}

void UBuffComponent::BuffJump(float BuffJumpVelocity, float BuffTime)
{
	if (Character == nullptr) return;

	//using timer handle to reset character speeds back to normal after time run out
	Character->GetWorldTimerManager().SetTimer(JumpBuffTimer, this, &UBuffComponent::ResetJump, BuffTime);

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->JumpZVelocity = BuffJumpVelocity;

	}

	MulticastBuffJump(BuffJumpVelocity);
}

void UBuffComponent::SetInitialJumpVelocity(float JumpVelocity)
{
	InitialJumpVelocity = JumpVelocity;
}

void UBuffComponent::MulticastBuffJump_Implementation(float JumpVelocity)
{
	Character->GetCharacterMovement()->JumpZVelocity = JumpVelocity;
}

void UBuffComponent::ResetJump()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	Character->GetCharacterMovement()->JumpZVelocity = InitialJumpVelocity;

	MulticastBuffJump(InitialJumpVelocity);
}

