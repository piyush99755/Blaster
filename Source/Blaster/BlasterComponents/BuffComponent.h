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

	bool bHealing = false;

	float HealingRate = 0.f;

	float AmountToHeal = 0.f;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Heal(float HealingAmount, float HealingTime);

	void HealRampUp(float DeltaTime);
};
