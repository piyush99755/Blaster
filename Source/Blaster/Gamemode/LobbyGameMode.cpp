// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFrameWork/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* PlayerController)
{
	Super::PostLogin(PlayerController);

	//getting number of players
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	if (NumberOfPlayers == 2)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			//if number of players = 2 , travel to blaster map. .
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/Maps/BlasterGameMap?listen"));
		}
	}
}
