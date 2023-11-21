// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
       DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	//this will get value of net role
	ENetRole RemoteRole = InPawn->GetLocalRole();
	FString Role;

	
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;

	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;

	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;

	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	//switch statement provides value of FString Role, that is used in display net role text
	FString RemoteRoleText = FString::Printf(TEXT("Local Role: %s"), *Role);

	//SetDisplayText(RemoteRoleText);

}

void UOverheadWidget::ShowPlayerName(APlayerController* NewPlayer)
{
	//getting player state to get get player name 
	APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
	if (PlayerState)
	{
		FString PlayerName = PlayerState->GetPlayerName();
		SetDisplayText(PlayerName);
	}
}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	//this function will remove from world, as soon as we travel to different level..
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}
