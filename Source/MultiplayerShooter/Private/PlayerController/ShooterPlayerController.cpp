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


void AShooterPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//GetWorld()->GameStateSetEvent.Add(this, &AShooterPlayerController::ReSetHUD);

	//GameStateSetDelegate.BindUObject(this, &AShooterPlayerController::ResetBindings);
	//GetWorld()->GameStateSetEvent.Add(GameStateSetDelegate);
	//ShooterGameState->OnMatchWarmupTick.AddDynamic(this, &AShooterHUD::UpdateAnnouncementWarmup);
}


void AShooterPlayerController::ReceivedGameModeClass(TSubclassOf<AGameModeBase> GameModeClass)
{
	Super::ReceivedGameModeClass(GameModeClass);

	UWorld* World = GetWorld();
	if (!World)
		return;

	/*Update bindings to game states*/
	ShooterHUD = Cast<AShooterHUD>(GetHUD());
	if (ShooterHUD)
	{
		if (AShooterGameState* ShooterGameState = World->GetGameState<AShooterGameState>())
		{
			ShooterHUD->ResetHUD(ShooterGameState);

			/*Send command to server when a player is ready to game*/
			ServerReadyToGame(ShooterGameState);

			ShooterGameState->OnMatchStateChanged.AddDynamic(this, &AShooterPlayerController::OnMatchStateChange);
		}
	}		
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AShooterPlayerController::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	Super::ClientSetHUD_Implementation(NewHUDClass);

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

//void AShooterPlayerController::ReceivedPlayer()
//{
//	Super::ReceivedPlayer();
//
//	if (IsLocalController()) RequestServerTimeFromClient(GetWorld()->GetTimeSeconds());
//}

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
	if (!ShooterHUD) return;
	
	ShooterHUD->UpdatePlayerScore(Value);
}

void AShooterPlayerController::UpdatePlayerDefeats(int32 Value)
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD) return;
	
	ShooterHUD->UpdatePlayerDefeats(Value);
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

void AShooterPlayerController::UpdateTopScorePlayer()
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD)
		return;

	ShooterHUD->UpdateTopScorePlayer();
}

void AShooterPlayerController::UpdateTopScore()
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (!ShooterHUD)
		return;

	ShooterHUD->UpdateTopScorePlayer();
}

void AShooterPlayerController::RefreshHUD()
{
	ShooterHUD = ShooterHUD ? ShooterHUD : Cast<AShooterHUD>(GetHUD());
	if (ShooterHUD) ShooterHUD->Refresh();
}

void AShooterPlayerController::ServerReadyToGame_Implementation(AShooterGameState* ShooterGameState)
{
	ShooterGameState->PlayerJoined();
}

void AShooterPlayerController::OnMatchStateChange(FName MatchState)
{
	if (MatchState == MatchState::InProgress)
	{
		if (AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState))
		{
			UpdatePlayerScore(ShooterPlayerState->GetScore());
			UpdatePlayerDefeats(ShooterPlayerState->GetDefeats());
		}
	}
}

//void AShooterPlayerController::RequestServerTimeFromClient_Implementation(float ClientRequestTime)
//{
//	ReportServerTimeToClient(ClientRequestTime, GetWorld()->GetTimeSeconds());
//}
//
//void AShooterPlayerController::ReportServerTimeToClient_Implementation(float ClientRequestTime, float ServerReportTime)
//{
//	const float CurrClientTime = GetWorld()->GetTimeSeconds();
//	const float TripRound = CurrClientTime - ClientRequestTime;
//	const float CurrServerTime = ServerReportTime + 0.5f * TripRound;
//	SyncDiffTime = CurrServerTime - CurrClientTime;
//}
//
//void AShooterPlayerController::CheckTimeSync(float DeltaTime)
//{
//	SyncRunningTime += DeltaTime;
//	if (IsLocalController() && SyncRunningTime > SyncFreq)
//	{
//		RequestServerTimeFromClient(GetWorld()->GetTimeSeconds());
//		SyncRunningTime = 0.f;
//	}
//}