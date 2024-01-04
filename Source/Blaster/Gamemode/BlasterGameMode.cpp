// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}
ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimmedPlayer,class ABlasterPlayerController* VictimController,  ABlasterPlayerController* AttackingController)
{
	//getting player state using player controllers as it has function to score functionality 
	ABlasterPlayerState* VictimPlayerState = VictimController? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	ABlasterPlayerState* AttackerPlayerState = AttackingController? Cast<ABlasterPlayerState>(AttackingController->PlayerState) : nullptr;
	

	//if (AttackerPlayerState != VictimPlayerState) UE_LOG(LogTemp, Warning, TEXT("Its victim"));
	//if (AttackerPlayerState == nullptr) UE_LOG(LogTemp, Warning, TEXT("Its null"));
	
	//adding score of 1 when attacker eliminates other player
	/*if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}*/

	if (ElimmedPlayer)
	{
		ElimmedPlayer->Elimination();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		//it will detach controller from character...
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}

	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

		//to get random value 
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);

		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
	
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	//actual time when map launches..
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		//calculating countdown time by substrating the time game starts on server from warm up time then
	    //adding level starting time to it which gives right amount of time 
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;


		//when countdown time reaches zero , match starts.. 
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		if (CountdownTime <= 0.f)
		{
			//when countdown time reaches zero , setting match state to cool down 
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	//as match state changes it will update it to all player controllers exist in the game level.. 
	for (FConstPlayerControllerIterator IT = GetWorld()->GetPlayerControllerIterator(); IT; ++IT)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*IT);
		if (BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState);
		}
	}
}
