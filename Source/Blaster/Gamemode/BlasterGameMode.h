// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

/**
 * 
 */

namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // match time ended and display winner 
}

UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	ABlasterGameMode();

	virtual void PlayerEliminated(class ABlasterCharacter* ElimmedPlayer, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackingController);
	
	virtual void RequestRespawn(ACharacter* Character, AController* Controller);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
		float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

	FORCEINLINE float GetCountdownTime() const {
		return CountdownTime;
	}

	


protected:

	virtual void BeginPlay()override;

	virtual void Tick(float DeltaTime)override; 

	virtual void OnMatchStateSet() override;

private:

	float CountdownTime = 0.f;

	 

};
