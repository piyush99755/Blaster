// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
		void MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

	UFUNCTION(BlueprintCallable)
		void MenuTearDown();

protected:


	virtual bool Initialize() override;

	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);

	//callback for custom delegate on MultiplayerSessionSubsystem
    UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);

	
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

	
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:

	//specify special type of specifier to bind button to the widget class 
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	//function to bind to the buttons

	UFUNCTION()
		void JoinButtonClicked();

	UFUNCTION()
		void HostButtonClicked();

	//multiplayer sessions subsystem class to handle online sessions functionality
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("FreeForAll") };
	FString PathToLobby{ TEXT("") };

	

	

};
