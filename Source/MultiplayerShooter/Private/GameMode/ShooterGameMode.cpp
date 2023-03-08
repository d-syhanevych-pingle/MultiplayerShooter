// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ShooterGameMode.h"
#include "Character/MainCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameState/ShooterGameState.h"
#include "PlayerController/ShooterPlayerController.h"
#include "PlayerState/ShooterPlayerState.h"

namespace MatchState
{
	const FName Cooldown = FName(TEXT("Cooldown"));
}

AShooterGameMode::AShooterGameMode()
{
	bDelayedStart = true;	
}

void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	TimerManager = &GetWorldTimerManager();
	TimerManager->SetTimer(TimerHandle_ChangeMatchState, this, &AShooterGameMode::StartCurrentMatch, WarmupTime, true);
}

void AShooterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AShooterGameMode::InitGameState()
{
	Super::InitGameState();

	AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>();
	if (!ShooterGameState) return;

}

void AShooterGameMode::FinishCurrentMatch()
{
	SetMatchState(MatchState::Cooldown);
	TimerManager->SetTimer(TimerHandle_ChangeMatchState, this, &AShooterGameMode::RestarthCurrentGame, CooldownTime, true);
}

void AShooterGameMode::RestarthCurrentGame()
{
	RestartGame();
	TimerManager->SetTimer(TimerHandle_ChangeMatchState, this, &AShooterGameMode::StartCurrentMatch, WarmupTime, true);
}

void AShooterGameMode::StartCurrentMatch()
{
	StartMatch();
	TimerManager->SetTimer(TimerHandle_ChangeMatchState, this, &AShooterGameMode::FinishCurrentMatch, MatchTime, true);
}

void AShooterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(*It))
		{
			ShooterPlayerController->OnMatchStateSet(MatchState);
		}
	}
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
	EliminatedCharacter->Eliminated();

	AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>();
	if (!ShooterGameState) return;

	// Update GameState Info
	ShooterGameState->UpdateTopScorePlayerStates(AttackerPlayerState);
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

