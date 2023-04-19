// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/ShooterGameState.h"

#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/ShooterPlayerController.h"
#include "PlayerState/ShooterPlayerState.h"
AShooterGameState::AShooterGameState()
{
	bReplicates = true;

	/*For new logged users*/
	/*if(!HasAuthority())
		StartTimer();*/
}
void AShooterGameState::UpdateTopScorePlayerStates(AShooterPlayerState* PlayerState)
{
	if (!PlayerState) return;
	
	if (TopScorePlayerStates.Num() == 0)
	{
		TopScorePlayerStates.AddUnique(PlayerState);
		TopScore = PlayerState->GetScore();
	}
	else if (TopScore == PlayerState->GetScore())
	{
		TopScorePlayerStates.AddUnique(PlayerState);
	}
	else if (TopScore < PlayerState->GetScore())
	{
		TopScorePlayerStates.Empty();
		TopScorePlayerStates.AddUnique(PlayerState);
		TopScore = PlayerState->GetScore();
	}
}

void AShooterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterGameState, MatchTime);
	DOREPLIFETIME(AShooterGameState, WarmupTime);
	DOREPLIFETIME(AShooterGameState, CooldownTime);
	DOREPLIFETIME(AShooterGameState, TopScorePlayerStates);
	DOREPLIFETIME(AShooterGameState, TopScore);
	DOREPLIFETIME(AShooterGameState, IsTimerStarted);
}

void AShooterGameState::StartTimer(bool IsStart)
{
	IsTimerStarted = IsStart;
}

void AShooterGameState::OnRep_IsTimerStarted()
{
	if (IsTimerStarted)
		DefaultTimer();
}

void AShooterGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

	if (GetNetMode() != NM_DedicatedServer)
	{
		OnMatchStateChanged.Broadcast(MatchState);
	}
}

void AShooterGameState::DefaultTimer()
{
	if (IsTimerStarted)
	{
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			++WarmupTime;
			OnMatchWarmupTick.Broadcast(WarmupTime);
		}
		if (GetMatchState() == MatchState::Cooldown)
		{
			++CooldownTime;
			OnMatchCooldownTick.Broadcast(CooldownTime);
		}
		if (IsMatchInProgress())
		{
			++MatchTime;
			OnMatchTick.Broadcast(MatchTime);
		}
	}

	Super::DefaultTimer();
}

void AShooterGameState::OnRep_TopScore()
{
	AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	
	if (!ShooterPlayerController || !ShooterPlayerController->IsLocalController())
		return;

	// Updating the TopScore in the HUD
	ShooterPlayerController->UpdateTopScore();
}

void AShooterGameState::OnRep_TopScorePlayerStates()
{
	AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (!ShooterPlayerController || !ShooterPlayerController->IsLocalController())
		return;

	// Updating the TopScorePlayer in the HUD
	ShooterPlayerController->UpdateTopScorePlayer();
}

void AShooterGameState::PlayerJoined()
{
	CountJoinedPlayers++;
	if (CountJoinedPlayers >= 2)
	{
		StartTimer(true);
	}
}
