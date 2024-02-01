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

	//health and ammo HUD functions
	UFUNCTION(NetMulticast, Reliable)
	void SetHealthHUD(float Health, float MaxHealth);

	//void SetScoreHUD(float Score);
	
	void SetWeaponAmmoHUD(int32 Ammo);

	
	void SetCarriedAmmoHUD(int32 Ammo);

     void SetGrenadeAmountHUD(int32 Grenades);

	 void PollInIt();

	 bool bInitializeCharacterOverlay = false;

	 float HUDHealth;

	 bool bInitializeHealth = false;

	 float HUDMaxHealth;

	 int32 HUDGrenade;

	 bool bInitializeGrenades = false;

	 int32 HUDWeaponAmmo;

	 bool bInitializeWeaponAmmo = false;

	 int32 HUDCarriedAmmo;

	 bool bInitializeCarriedAmmo = false;



	 //match state functions
	void SetMatchCountdownTime(float CountdownTime);

	void SetAnnouncementCountdownTime(float CountdownTime);

	void SetHUDTime();

	void OnMatchStateSet(FName State);

	void HandleMatchHasStarted();

	void HandleCooldown();

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



	//high ping functions and variables
	void HighPingWarning();

	void StopHighPingWarning();

	void CheckPing(float DeltaTime);

	float HighPingRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;

	float CheckPingFrequency = 20.f;

	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;

	float PingAnimationRunningTime = 0.f;
	
	//class variables

	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	UPROPERTY()
	class ABlasterCharacter* BlasterCharacter;

	UPROPERTY()
		class ABlasterGameMode* BlasterGameMode;

	UPROPERTY()
		class UCharacterOverlay* CharacterOverlay;

	

	

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
