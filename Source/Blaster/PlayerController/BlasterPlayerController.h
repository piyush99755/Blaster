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
	UFUNCTION(NetMulticast, Reliable)
	void SetHealthHUD(float Health, float MaxHealth);

	//void SetScoreHUD(float Score);

	void SetWeaponAmmoHUD(int32 Ammo);

	void SetCarriedAmmoHUD(int32 Ammo);

	void SetMatchCountdownTime(float CountdownTime);

	void SetAnnouncementCountdownTime(float CountdownTime);

	void SetHUDTime();

	void OnMatchStateSet(FName State);

	void HandleMatchHasStarted();

	void HandleCooldown();
	


	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	UPROPERTY()
	class ABlasterCharacter* BlasterCharacter;

	UPROPERTY()
		class ABlasterGameMode* BlasterGameMode;

	UPROPERTY()
		class UCharacterOverlay* CharacterOverlay;

	float MatchTime = 0.f;

	float WarmupTime = 0.f;

	float LevelStartingTime = 0.f;

	float CooldownTime = 0.f;

	uint32 CountdownInt = 0;

	float ServerClientDelta = 0.f;

	//every 5 seconds synced time between server and client..
	UPROPERTY(EditAnywhere)
	float TimeSyncedFrequency = 5.f;

	float TimeSyncedRunningTime = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	void PollInIt();

	bool bInitializeCharacterOverlay = false;

	float HUDHealth;

	float HUDMaxHealth;

protected:

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime)override;

	virtual void OnPossess(APawn* InPawn)override;

	virtual void ReceivedPlayer() override; 


	/**
	* sync time between client and server
	*/
	//request the current server time, passing in the client's time when request was sent..
	UFUNCTION(Server, Reliable)
		void ServerRequestServerTime(float TimeOfClientRequest);


	//reports the current server time to client in response to ServerRequestServerTime..
	UFUNCTION(Client, Reliable)
		void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	virtual float GetServerTime(); 

	void CheckTimeSync(float DeltaTime);


	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

	
	
};
