// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

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
