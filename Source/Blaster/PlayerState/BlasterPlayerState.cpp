// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"


//this function is for server side
/*
void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	Score += ScoreAmount;
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetScoreHUD(Score);
		}
	}
}


//this function is for client side
void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetScoreHUD(Score);
		}
	}
}*/
