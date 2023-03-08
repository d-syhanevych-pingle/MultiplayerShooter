// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ServerGameModeBase.h"
#include "OnlineSubsystem.h"

void AServerGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	CreateSession();
}

void AServerGameModeBase::CreateSession()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(); 
	
	if (Subsystem)
	{
		OnlineSessionInterface = Subsystem->GetSessionInterface();
	}

	if (!OnlineSessionInterface.IsValid())
		return;

	OnlineSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &AServerGameModeBase::OnCreateSessionComplete);
	// Create a new Session
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;	// Set up a session over the Internet or Local
	LastSessionSettings->NumPublicConnections = NumPublicConnections;	// Allow the number of players in the game
	LastSessionSettings->bAllowJoinInProgress = true;					// Allow players to join in when the program is running
	LastSessionSettings->bAllowJoinViaPresence = true;					// Steam's presence--searching for players in his region of the world
	LastSessionSettings->bShouldAdvertise = true;						// Allow other players to find the session by the advertise
	LastSessionSettings->bUsesPresence = true;							// Also like bAllowJoinViaPresence
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->BuildUniqueId = 1;

	// Set up a match type
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// Check the session, if created in failure, then remove the delegate from the delegate list ('OnCreateSessionComplete' won't be called)
	// and we broadcast the custom delegate.
	// These operations will also be done when we created the session successfully, but here we do is just for 'early time'
	if (!OnlineSessionInterface->CreateSession(0, NAME_GameSession, *LastSessionSettings))
	{
		return;
	}
}

void AServerGameModeBase::OnCreateSessionComplete(FName ServerName, bool IsSuccess)
{
	if (!IsSuccess)
		return;

	// Server Travel to the Lobby Level
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel(LobbyPath);
	}
}

void AServerGameModeBase::DestroySession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	// Destroy the session
	OnlineSessionInterface->DestroySession(NAME_GameSession);

}