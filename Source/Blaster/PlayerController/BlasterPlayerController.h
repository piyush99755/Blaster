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

	void SetMatchCountdownTime(float CountdownTime);

	void SetHUDTime();

	void OnMatchStateSet(FName State);
	


	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	UPROPERTY()
	class ABlasterCharacter* BlasterCharacter;

	UPROPERTY()
		class UCharacterOverlay* CharacterOverlay;

	float MatchTime = 120.f;

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

	
	
};
