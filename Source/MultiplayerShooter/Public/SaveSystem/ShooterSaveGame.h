// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ShooterSaveGame.generated.h"

USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()

public:
	/* Player Id defined by the online sub system (such as Steam) converted to FString for simplicity  */
	UPROPERTY()
	FString PlayerID;

	UPROPERTY()
	float Score;

	UPROPERTY()
	int32 Defeats;
};


/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API UShooterSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FPlayerSaveData> SavedPlayers;

	FPlayerSaveData* GetPlayerData(APlayerState* PlayerState);
};
