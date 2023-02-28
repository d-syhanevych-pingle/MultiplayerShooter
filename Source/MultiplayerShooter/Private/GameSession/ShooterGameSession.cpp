// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSession/ShooterGameSession.h"
#include "GameMode/ShooterGameMode.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

void AShooterGameSession::RegisterServer()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		return;
	}

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		return;
	}

	FNamedOnlineSession* Session = SessionInterface->GetNamedSession(GameSessionName);
	if (!Session)
	{
		return;
	}

	// Update the session
	FOnlineSessionSettings SessionSettings = Session->SessionSettings;
	if (AShooterGameMode* GameMode = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
	{
		SessionSettings.NumPublicConnections = GameMode->GetMaxCountOfPlayes();
		SessionInterface->UpdateSession(GameSessionName, SessionSettings, true);
	}
}