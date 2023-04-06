// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSystem/ShooterGameInstanceSubsystem.h"
#include "PlayerState/ShooterPlayerState.h"
#include "SaveSystem/ShooterSaveGame.h"
#include "SaveSystem/ShooterDeveloperSettings.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

void UShooterGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UShooterDeveloperSettings* SGSettings = GetDefault<UShooterDeveloperSettings>();
	// Access defaults from DefaultGame.ini
	CurrentSlotName = SGSettings->SaveSlotName;

	// Make sure it's loaded into memory .Get() only resolves if already loaded previously elsewhere in code
	UDataTable* DummyTable = SGSettings->DummyTablePath.LoadSynchronous();
	//DummyTable->GetAllRows() // We don't need this table for anything, just an content reference example
}

void UShooterGameInstanceSubsystem::SetSlotName(FString NewSlotName)
{
	// Ignore empty name
	if (NewSlotName.Len() == 0)
	{
		return;
	}

	CurrentSlotName = NewSlotName;
}

void UShooterGameInstanceSubsystem::WriteSaveGame()
{
	// Clear arrays, may contain data from previously loaded SaveGame
	CurrentSaveGame->SavedPlayers.Empty();

	AGameStateBase* BaseGameState = GetWorld()->GetGameState();
	if (BaseGameState == nullptr)
		return;

	for (int32 i = 0; i < BaseGameState->PlayerArray.Num(); i++)
	{
		AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(BaseGameState->PlayerArray[i]);
		if (ShooterPlayerState)
		{
			ShooterPlayerState->SavePlayerState(CurrentSaveGame);
		}
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);

	OnSaveGameWritten.Broadcast(CurrentSaveGame);
}

void UShooterGameInstanceSubsystem::LoadSaveGame(FString InSlotName)
{
	// Update slot name first if specified, otherwise keeps default name
	SetSlotName(InSlotName);

	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{
		CurrentSaveGame = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
		if (CurrentSaveGame == nullptr)
			return;

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));

		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	}
	else
	{
		CurrentSaveGame = Cast<UShooterSaveGame>(UGameplayStatics::CreateSaveGameObject(UShooterSaveGame::StaticClass()));

		UE_LOG(LogTemp, Log, TEXT("Created New SaveGame Data."));
	}
}


void UShooterGameInstanceSubsystem::HandleStartingNewPlayer(AController* NewPlayer)
{
	if (AShooterPlayerState* ShooterPlayerState = NewPlayer->GetPlayerState<AShooterPlayerState>())
	{
		ShooterPlayerState->LoadPlayerState(CurrentSaveGame);
	}
}
