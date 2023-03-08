// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ServerGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AServerGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

private:

	IOnlineSessionPtr OnlineSessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("FreeForAll_Shooter") };
	FString LobbyPath{ "/Game/Maps/Lobby" };

	UFUNCTION()
	void CreateSession();
	UFUNCTION()
	void DestroySession();
	void OnCreateSessionComplete(FName ServerName, bool IsSuccess);
};
