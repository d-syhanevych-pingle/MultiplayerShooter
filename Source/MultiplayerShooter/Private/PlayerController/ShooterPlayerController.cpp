// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/ShooterPlayerController.h"
#include "Character/MainCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerState.h"
#include "PlayerState/ShooterPlayerState.h"
#include "GameMode/ShooterGameMode.h"
#include "GameState/ShooterGameState.h"
#include "HUD/AnnouncementWidget.h"
#include "HUD/ShooterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ShooterHUD = Cast<AShooterHUD>(GetHUD());

	if (GetNetMode() != NM_DedicatedServer)
	{
		if (AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this)))
		{
			ShooterGameState->OnMatchWarmupTick.AddDynamic(this, &AShooterPlayerController::UpdateAnnouncementWarmup);
			ShooterGameState->OnMatchTick.AddDynamic(this, &AShooterPlayerController::UpdateMatchCountDown);
			ShooterGameState->OnMatchCooldownTick.AddDynamic(this, &AShooterPlayerController::UpdateAnnouncementCooldown);
			ServerGetMatchState();
		}
	}
}

void AShooterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//CheckTimeSync(DeltaTime);
}

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(InPawn))
	{
		MainCharacter->SetIsRespawned();
	}
	if (HasAuthority()) 
	{
		OnPawnPosses.Broadcast(this);
	}
	else
	{
		ServerPawnPosses();
	}
}

void AShooterPlayerController::ServerPawnPosses_Implementation()
{
	OnPawnPosses.Broadcast(this);
}

void AShooterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController()) RequestServerTimeFromClient(GetWorld()->GetTimeSeconds());
}

void AShooterPlayerController::UpdatePlayerHealth(float Health, float MaxHealth)
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD || !ShooterHUD->GetCharacterOverlay() || !ShooterHUD->GetCharacterOverlay()->HealthBar ||
		!ShooterHUD->GetCharacterOverlay()->HealthText) return;
	
	ShooterHUD->GetCharacterOverlay()->HealthBar->SetPercent(Health / MaxHealth);
	const FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
	ShooterHUD->GetCharacterOverlay()->HealthText->SetText(FText::FromString(HealthText));
}

void AShooterPlayerController::UpdatePlayerScore(float Value)
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD || !ShooterHUD->GetCharacterOverlay() || !ShooterHUD->GetCharacterOverlay()->Score) return;
	
	const FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Value));
	ShooterHUD->GetCharacterOverlay()->Score->SetText(FText::FromString(ScoreText));
}

void AShooterPlayerController::UpdatePlayerDefeats(int32 Value)
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD || !ShooterHUD->GetCharacterOverlay() || !ShooterHUD->GetCharacterOverlay()->Defeats) return;
	
	const FString DefeatsText = FString::Printf(TEXT("%d"), Value);
	ShooterHUD->GetCharacterOverlay()->Defeats->SetText(FText::FromString(DefeatsText));
}

void AShooterPlayerController::DisplayDefeatedMsg()
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD || !ShooterHUD->GetCharacterOverlay() || !ShooterHUD->GetCharacterOverlay()->DefeatedMsg ||
		!ShooterHUD->GetCharacterOverlay()->DefeatedMsgAnim) return;

	UCharacterOverlay* CharacterOverlay = ShooterHUD->GetCharacterOverlay();
	CharacterOverlay->DefeatedMsg->SetVisibility(ESlateVisibility::Visible);
	CharacterOverlay->PlayAnimation(CharacterOverlay->DefeatedMsgAnim);
}

void AShooterPlayerController::UpdateWeaponAmmo(int32 AmmoAmount)
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD || !ShooterHUD->GetCharacterOverlay() || !ShooterHUD->GetCharacterOverlay()->WeaponAmmoAmount) return;
	
	const FString AmmoText = FString::Printf(TEXT("%d"), AmmoAmount);
	ShooterHUD->GetCharacterOverlay()->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
}

void AShooterPlayerController::UpdateCarriedAmmo(int32 AmmoAmount)
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD || !ShooterHUD->GetCharacterOverlay() || !ShooterHUD->GetCharacterOverlay()->CarriedAmmoAmount) return;
	
	const FString AmmoText = FString::Printf(TEXT("%d"), AmmoAmount);
	ShooterHUD->GetCharacterOverlay()->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
}

void AShooterPlayerController::UpdateWeaponType(const FString& WeaponType)
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD || !ShooterHUD->GetCharacterOverlay() || !ShooterHUD->GetCharacterOverlay()->WeaponType) return;
	
	ShooterHUD->GetCharacterOverlay()->WeaponType->SetText(FText::FromString(WeaponType));
}

void AShooterPlayerController::UpdateGrenade(int32 GrenadeAmount)
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD || !ShooterHUD->GetCharacterOverlay() || !ShooterHUD->GetCharacterOverlay()->GrenadeAmount) return;

	const FString GrenadeAmountStr = FString::Printf(TEXT("%d"), GrenadeAmount);
	ShooterHUD->GetCharacterOverlay()->GrenadeAmount->SetText(FText::FromString(GrenadeAmountStr));
}

void AShooterPlayerController::UpdateAnnouncementWarmup(int32 CurrentTime)
{
	UE_LOG(LogTemp, Display, TEXT("Change warmup to %d"), CurrentTime);
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD)
		return;

	if (CurrentTime <= WarmupTime && CurrentTime >= 0)
		ShooterHUD->UpdateAnnouncement(WarmupTime - CurrentTime);
}

void AShooterPlayerController::UpdateAnnouncementCooldown(int32 CurrentTime)
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD)
		return;

	if (CurrentTime <= CooldownTime && CurrentTime >= 0)
		ShooterHUD->UpdateAnnouncement(CooldownTime - CurrentTime);
}

void AShooterPlayerController::UpdateMatchCountDown(int32 CurrentTime)
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD || !ShooterHUD->GetCharacterOverlay() || !ShooterHUD->GetCharacterOverlay()->MatchCountdown) return;
	int32 Countdown = MatchTime - CurrentTime;

	UCharacterOverlay* CharacterOverlay = ShooterHUD->GetCharacterOverlay();
	if (Countdown > 0 && Countdown <= 30)
	{
		// Urgent countdown effect, turns red and play blink animation. (animation no need to loop, because update is loop every 1 second)
		CharacterOverlay->MatchCountdown->SetColorAndOpacity((FLinearColor(1.f, 0.f, 0.f)));
		CharacterOverlay->PlayAnimation(CharacterOverlay->TimeBlink);
	}
	else if (Countdown <= 0)
	{
		CharacterOverlay->MatchCountdown->SetText(FText());
		CharacterOverlay->MatchCountdown->SetColorAndOpacity((FLinearColor(1.f, 1.f, 1.f)));
		CharacterOverlay->StopAnimation(CharacterOverlay->TimeBlink);
		return;
	}
	const int32 Minute = Countdown / 60.f;
	const int32 Second = Countdown - 60 * Minute;
	const FString MatchCountdown = FString::Printf(TEXT("%02d:%02d"), Minute, Second);
	CharacterOverlay->MatchCountdown->SetText(FText::FromString(MatchCountdown));
}

void AShooterPlayerController::UpdateTopScorePlayer()
{
	const AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	if (!ShooterGameState) return;

	auto PlayerStates = ShooterGameState->GetTopScorePlayerStates();
	if (PlayerStates.IsEmpty()) return;

	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD || !ShooterHUD->GetCharacterOverlay() || !ShooterHUD->GetCharacterOverlay()->TopScorePlayer) return;
	
	FString PlayerName;
	for (const auto& State: PlayerStates)
	{
		if (!State) return;
		PlayerName.Append(FString::Printf(TEXT("%s\n"), *State->GetPlayerName()));
	}
	ShooterHUD->GetCharacterOverlay()->TopScorePlayer->SetText(FText::FromString(PlayerName));
}

void AShooterPlayerController::UpdateTopScore()
{
	const AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	if (!ShooterGameState) return;

	const auto PlayerStates = ShooterGameState->GetTopScorePlayerStates();
	const float TopScore = ShooterGameState->GetTopScore();
	if (PlayerStates.IsEmpty()) return;
	
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD || !ShooterHUD->GetCharacterOverlay() || !ShooterHUD->GetCharacterOverlay()->TopScore) return;
	
	FString TopScoreString;
	for (int32 i = 0; i < PlayerStates.Num(); ++i)
	{
		TopScoreString.Append(FString::Printf(TEXT("%d\n"), FMath::CeilToInt32(TopScore)));
	}
	ShooterHUD->GetCharacterOverlay()->TopScore->SetText(FText::FromString(TopScoreString));
}

void AShooterPlayerController::RefreshHUD()
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (ShooterHUD) ShooterHUD->Refresh();
}

void AShooterPlayerController::OnMatchStateSet(FName State)
{
	if (MatchState == State)
		return;
	MatchState = State;
	if (GetNetMode() != NM_DedicatedServer)
	{
		ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
		if (ShooterHUD)
			ShooterHUD->HandleMatchState(MatchState);
	}
}

void AShooterPlayerController::RequestServerTimeFromClient_Implementation(float ClientRequestTime)
{
	ReportServerTimeToClient(ClientRequestTime, GetWorld()->GetTimeSeconds());
}

void AShooterPlayerController::ReportServerTimeToClient_Implementation(float ClientRequestTime, float ServerReportTime)
{
	const float CurrClientTime = GetWorld()->GetTimeSeconds();
	const float TripRound = CurrClientTime - ClientRequestTime;
	const float CurrServerTime = ServerReportTime + 0.5f * TripRound;
	SyncDiffTime = CurrServerTime - CurrClientTime;
}

void AShooterPlayerController::CheckTimeSync(float DeltaTime)
{
	SyncRunningTime += DeltaTime;
	if (IsLocalController() && SyncRunningTime > SyncFreq)
	{
		RequestServerTimeFromClient(GetWorld()->GetTimeSeconds());
		SyncRunningTime = 0.f;
	}
}

void AShooterPlayerController::ServerGetMatchState_Implementation()
{
	const AShooterGameMode* ShooterGameMode = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode());
	if (!ShooterGameMode) return;

	ClientJoinMidGame(ShooterGameMode->GetWarmupTime(), ShooterGameMode->GetMatchTime(),
		ShooterGameMode->GetCooldownTime(), ShooterGameMode->GetMatchState());
}

void AShooterPlayerController::ClientJoinMidGame_Implementation(float Warmup, float Match, float Cooldown, FName State)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;

	// If the player is joining mid-game and the game is now in progress, the UI should switch to the MatchState's UI, so the
	// player should be notified which game state is now going on.
	OnMatchStateSet(State);

	if (AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this)))
		ShooterGameState->OnMatchStateChanged.AddDynamic(this, &AShooterPlayerController::OnMatchStateSet);
}