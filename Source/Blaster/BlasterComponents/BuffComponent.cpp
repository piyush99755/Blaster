// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"
#include "Blaster/Character/BlasterCharacter.h"


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

