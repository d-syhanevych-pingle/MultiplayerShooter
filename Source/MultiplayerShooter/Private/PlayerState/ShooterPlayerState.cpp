// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ShooterPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/ShooterPlayerController.h"

void AShooterPlayerState::UpdateScore()
{
	SetScore(GetScore() + ScoreAmount);
	OnRep_Score();
}

void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerState, Defeats);
}

void AShooterPlayerState::OnRep_Score()
{
	ShooterPlayerController = ShooterPlayerController ? ShooterPlayerController : Cast<AShooterPlayerController>(GetOwningController());
	if (!ShooterPlayerController) return;

	ShooterPlayerController->UpdatePlayerScore(GetScore());
}

void AShooterPlayerState::UpdateDefeats()
{
	Defeats += 1;
	OnRep_Defeats();
}

void AShooterPlayerState::OnRep_Defeats()
{
	ShooterPlayerController = ShooterPlayerController ? ShooterPlayerController : Cast<AShooterPlayerController>(GetOwningController());
	if (!ShooterPlayerController) return;

	ShooterPlayerController->UpdatePlayerDefeats(Defeats);
}

void AShooterPlayerState::SavePlayerState_Implementation(UShooterSaveGame* SaveObject)
{
	if (SaveObject)
	{
		// Gather all relevant data for player
		FPlayerSaveData SaveData;
		SaveData.Score = GetScore();
		SaveData.Defeats = Defeats;
		// Stored as FString for simplicity (original Steam ID is uint64)
		SaveData.PlayerID = GetUniqueId().ToString();

		SaveObject->SavedPlayers.Add(SaveData);
	}
}

void AShooterPlayerState::LoadPlayerState_Implementation(UShooterSaveGame* SaveObject)
{
	if (SaveObject)
	{
		FPlayerSaveData* FoundData = SaveObject->GetPlayerData(this);
		if (FoundData)
		{
			//Credits = SaveObject->Credits;
			// Makes sure we trigger credits changed event
			SetScore(FoundData->Score);
			Defeats = FoundData->Defeats;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find SaveGame data for player id '%i'."), GetPlayerId());
		}
	}
}