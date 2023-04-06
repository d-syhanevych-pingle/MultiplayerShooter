// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ShooterGameMode.h"
#include "Character/MainCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "SaveSystem/ShooterGameInstanceSubsystem.h"
#include "DeveloperSettings/GameMapsModesDeveloperSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameState/ShooterGameState.h"
#include "PlayerController/ShooterPlayerController.h"
#include "PlayerState/ShooterPlayerState.h"
#include "Net/UnrealNetwork.h"

namespace MatchState
{
	const FName Cooldown = FName(TEXT("Cooldown"));
}

AShooterGameMode::AShooterGameMode()
{
	bDelayedStart = true;	
}

void AShooterGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// (Save/Load logic moved into ShooterGameInstanceSubsystem)
	UShooterGameInstanceSubsystem* ShooterGameInstanceSubsystem = GetGameInstance()->GetSubsystem<UShooterGameInstanceSubsystem>();

	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "ShooterSavedGame");
	ShooterGameInstanceSubsystem->LoadSaveGame(SelectedSaveSlot);
}

void AShooterGameMode::InitGameState()
{
	Super::InitGameState();

	if (AShooterGameState* ShooterGameState = Cast<AShooterGameState>(GameState))
	{
		ShooterGameState->OnMatchWarmupTick.AddDynamic(this, &AShooterGameMode::StartCurrentMatch);
		ShooterGameState->OnMatchTick.AddDynamic(this, &AShooterGameMode::FinishCurrentMatch);
		ShooterGameState->OnMatchCooldownTick.AddDynamic(this, &AShooterGameMode::RestartCurrentGame);
	}
}

void AShooterGameMode::PostLogin(APlayerController* NewPlayer) 
{
	Super::PostLogin(NewPlayer);

	/*const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers >= 2 && !TimerHandle_ChangeMatchState.IsValid())
	{
		if (AShooterGameState* ShooterGameState = Cast<AShooterGameState>(GameState))
			ShooterGameState->StartTimer();
	}*/
}
void AShooterGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	//const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	//if (NumberOfPlayers >= 2 && !TimerHandle_ChangeMatchState.IsValid())
	//{
	//	if (AShooterGameState* ShooterGameState = Cast<AShooterGameState>(GameState))
	//		ShooterGameState->StartTimer();
	//}
}

void AShooterGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	UShooterGameInstanceSubsystem* ShooterGameInstanceSubsystem = GetGameInstance()->GetSubsystem<UShooterGameInstanceSubsystem>();
	ShooterGameInstanceSubsystem->HandleStartingNewPlayer(NewPlayer);

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

}

void AShooterGameMode::StartCurrentMatch(int32 Countdown)
{
	if (Countdown >= WarmupTime)
		StartMatch();
}

void AShooterGameMode::FinishCurrentMatch(int32 Countdown)
{
	if (Countdown >= MatchTime)
		SetMatchState(MatchState::Cooldown);
}

void AShooterGameMode::RestartCurrentGame(int32 Countdown)
{
	if (Countdown >= CooldownTime)
	{
		bUseSeamlessTravel = true;
		FString TravelURL = GetRandomTravelPath();
		UE_LOG(LogTemp, Log, TEXT("ServerTravel - Travel to: %s"), *TravelURL);

		UWorld* World = GetWorld();
		if (World)
			World->ServerTravel(TravelURL);
	}
	
}

const FString AShooterGameMode::GetRandomTravelPath()
{
	/* Get Random Map */
	const TArray<FMapDescription> AvaliableMaps = GetDefault<UGameMapsModesDeveloperSettings>()->AvaliableMaps;
	int32 MapIndex = FMath::RandRange(0, AvaliableMaps.Num() - 1);
	FString MapPath = AvaliableMaps[MapIndex].Map.GetLongPackageName();

	/* Get Random GameMode by Map*/
	const TArray<TSoftClassPtr<AShooterGameMode>> AvaliableGameModes = AvaliableMaps[MapIndex].AvaliableGameModes;
	int32 GameModeIndex = FMath::RandRange(0, AvaliableGameModes.Num() - 1);
	FString GameModeName = AvaliableGameModes[GameModeIndex].GetAssetName();
	GameModeName = AvaliableGameModes[GameModeIndex].GetLongPackageName();
	GameModeName = AvaliableGameModes[GameModeIndex].ToSoftObjectPath().GetAssetPathString();

	return FString::Printf(TEXT("%s?game=%s?listen"), *MapPath, *GameModeName);
}

void AShooterGameMode::PlayerEliminated(AMainCharacter* EliminatedCharacter, AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController)
{
	if (!EliminatedCharacter || !AttackerController || !VictimController) return;

	AShooterPlayerState* AttackerPlayerState = AttackerController->GetPlayerState<AShooterPlayerState>();
	AShooterPlayerState* VictimPlayerState = VictimController->GetPlayerState<AShooterPlayerState>();
	if (!AttackerPlayerState || !VictimPlayerState) return;

	// Need to check if it's suicide.
	if (AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->UpdateScore();
		VictimPlayerState->UpdateDefeats();
	}
	EliminatedCharacter->MulticastEliminated();

	AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>();
	if (!ShooterGameState) return;

	// Update GameState Info
	ShooterGameState->UpdateTopScorePlayerStates(AttackerPlayerState);

	UShooterGameInstanceSubsystem* ShooterGameInstanceSubsystem = GetGameInstance()->GetSubsystem<UShooterGameInstanceSubsystem>();
	// Immediately auto save on death
	ShooterGameInstanceSubsystem->WriteSaveGame();
}

void AShooterGameMode::RequestRespawn(AMainCharacter* EliminatedCharacter, AController* EliminatedController)
{
	// Detach character from the controller and destroy.
	if (!EliminatedCharacter) return;
	EliminatedCharacter->Reset();
	EliminatedCharacter->Destroy();

	// Respawn a new character with a random reborn-spot for the controller.
	if (!EliminatedController) return;
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	const int32 PlayerStartIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);

	RestartPlayerAtPlayerStart(EliminatedController, PlayerStarts[PlayerStartIndex]);
}

