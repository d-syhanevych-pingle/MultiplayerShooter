// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ShooterHUD.h"
#include "Components/TextBlock.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/AnnouncementWidget.h"
#include "Character/MainCharacter.h"
#include "GameState/ShooterGameState.h"
#include "GameMode/ShooterGameMode.h"
#include "PlayerState/ShooterPlayerState.h"
#include "ShooterComponents/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/ShooterPlayerController.h"

// The DrawHUD function will be automatically called when we set the default HUD as BP_ShooterHUD in BP_GameMode settings.
void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();
	
	if (GEngine && GEngine->GameViewport)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		ViewportCenter = ViewportSize * .5f;
		DrawCrosshairs(HUDPackage.CrosshairsCenter, FVector2D(0.f, 0.f));
		DrawCrosshairs(HUDPackage.CrosshairsLeft, FVector2D(-HUDPackage.CrosshairsCurrentSpread, 0.f));
		DrawCrosshairs(HUDPackage.CrosshairsRight, FVector2D(HUDPackage.CrosshairsCurrentSpread, 0.f));
		DrawCrosshairs(HUDPackage.CrosshairsTop, FVector2D(0.f, -HUDPackage.CrosshairsCurrentSpread));
		DrawCrosshairs(HUDPackage.CrosshairsBottom, FVector2D(0.f, HUDPackage.CrosshairsCurrentSpread));
	}
}

void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();

	//ResetHUD();
	//AddAnnouncement();
}

void AShooterHUD::ResetHUD(AShooterGameState* ShooterGameState)
{
	if (const AShooterGameMode* ShooterGameMode = Cast<AShooterGameMode>(ShooterGameState->GetDefaultGameMode()))
	{
		WarmupTime = ShooterGameMode->GetWarmupTime();
		MatchTime = ShooterGameMode->GetMatchTime();
		CooldownTime = ShooterGameMode->GetCooldownTime();
	}
	ShooterGameState->OnMatchWarmupTick.AddDynamic(this, &AShooterHUD::UpdateAnnouncementWarmup);
	ShooterGameState->OnMatchTick.AddDynamic(this, &AShooterHUD::UpdateMatchCountDown);
	ShooterGameState->OnMatchCooldownTick.AddDynamic(this, &AShooterHUD::UpdateAnnouncementCooldown);
	ShooterGameState->OnMatchStateChanged.AddDynamic(this, &AShooterHUD::HandleMatchState);
}

void AShooterHUD::UpdateAnnouncementWarmup(int32 CurrentTime)
{
	if (CurrentTime <= WarmupTime && CurrentTime >= 0)
		UpdateAnnouncement(WarmupTime - CurrentTime);
}

void AShooterHUD::UpdateAnnouncementCooldown(int32 CurrentTime)
{
	if (CurrentTime <= CooldownTime && CurrentTime >= 0)
		UpdateAnnouncement(CooldownTime - CurrentTime);
}

void AShooterHUD::AddCharacterOverlay()
{
	if (CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(GetWorld(), CharacterOverlayClass, FName("Character Overlay"));
		if (!CharacterOverlay) return;

		Refresh();
		CharacterOverlay->AddToViewport();
	}
}

void AShooterHUD::AddAnnouncement()
{
	if (AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncementWidget>(GetWorld(), AnnouncementClass, FName("Announcement"));
		if (!Announcement) return;
		
		Announcement->AddToViewport();
	}
}

void AShooterHUD::Refresh()
{
	if (CharacterOverlay && CharacterOverlay->DefeatedMsg)
	{
		CharacterOverlay->DefeatedMsg->SetVisibility(ESlateVisibility::Hidden);
		if (CharacterOverlay->IsAnimationPlaying(CharacterOverlay->DefeatedMsgAnim))
		{
			CharacterOverlay->StopAnimation(CharacterOverlay->DefeatedMsgAnim);
		}
	}
	// We need player controller and player character because the data is stored there.
	if (AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(GetOwningPlayerController()))
	{
		ShooterPlayerController->UpdatePlayerHealth(100.f, 100.f);
		const AMainCharacter* MainCharacter = Cast<AMainCharacter>(ShooterPlayerController->GetCharacter());
		if (MainCharacter && MainCharacter->GetCombat())
		{
			ShooterPlayerController->UpdateGrenade(MainCharacter->GetCombat()->GetGrenadeAmount());
		}
	}
}

void AShooterHUD::DrawCrosshairs(UTexture2D* Texture, const FVector2D& Spread)
{
	if (!Texture) return;
	
	DrawTexture(
		Texture,
		ViewportCenter.X - Texture->GetSizeX() * .5f + Spread.X,
		ViewportCenter.Y - Texture->GetSizeY() * .5f + Spread.Y,
		Texture->GetSizeX(),
		Texture->GetSizeY(),
		0.f,
		0.f,
		1.f,
		1.f,
		HUDPackage.CrosshairsColor
	);
}

void AShooterHUD::HandleMatchState(FName MatchStateIn)
{
	if (MatchState == MatchStateIn)
		return;
	MatchState = MatchStateIn;

	if (MatchState == MatchState::InProgress)
	{
		if (Announcement)
		{
			Announcement->SetVisibility(ESlateVisibility::Hidden);
		}

		if (!CharacterOverlay) AddCharacterOverlay();

		if (CharacterOverlay)
		{
			if (const AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(GetOwningPlayerController()))
			{
				UpdateMatchCountDown(MatchTime);			
			}
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		if (!CharacterOverlay || !Announcement ||
			!Announcement->Announce_0 || !Announcement->Announce_1 ||
			!Announcement->WinText) return;

		CharacterOverlay->RemoveFromParent();
		Announcement->Announce_0->SetText(FText::FromString("New Match Starts in:"));
		Announcement->Announce_1->SetText(FText::FromString(""));
		if (const AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(GetOwningPlayerController()))
		{
			UpdateAnnouncement(CooldownTime);
		}
		Announcement->SetVisibility(ESlateVisibility::Visible);

		// When the match ends, we show the winner announcement.
		
		if (const AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this)))
		{
			if (const APlayerController* PlayerController = GetOwningPlayerController())
			{
				if (const AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerController->PlayerState))
				{
					FString WinString;
					auto PlayerStates = ShooterGameState->GetTopScorePlayerStates();
					if (PlayerStates.Num() == 0)
					{
						WinString = "There is no winner.";
					}
					else if (PlayerStates.Num() == 1 && PlayerStates[0] == ShooterPlayerState)
					{
						WinString = "You are the winner!";
					}
					else if (PlayerStates.Num() == 1)
					{
						WinString = "Winner:\n";
						WinString.Append(FString::Printf(TEXT("%s\n"), *PlayerStates[0]->GetPlayerName()));
					}
					else if (PlayerStates.Num() > 1)
					{
						WinString = "Players tied for the win:\n";
						for (const auto& State : PlayerStates)
						{
							WinString.Append(FString::Printf(TEXT("%s\n"), *State->GetPlayerName()));
						}
					}
					Announcement->WinText->SetText(FText::FromString(WinString));
					Announcement->WinText->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}	
	}
	else if (MatchState == MatchState::WaitingToStart)
	{
		if (!Announcement)
			AddAnnouncement();

		if (const AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(GetOwningPlayerController()))
		{
			UpdateAnnouncement(WarmupTime);
		}
	}
}

void AShooterHUD::UpdateAnnouncement(int32 Countdown)
{
	if (!Announcement || !Announcement->Announce_0 ||
		!Announcement->Announce_1 || !Announcement->TimeText) return;

	if (Countdown <= 0)
	{
		Announcement->Announce_0->SetText(FText());
		Announcement->Announce_1->SetText(FText());
		Announcement->TimeText->SetText(FText());
		return;
	}
	const int32 Minute = Countdown / 60.f;
	const int32 Second = Countdown - 60 * Minute;
	const FString CountdownString = FString::Printf(TEXT("%02d:%02d"), Minute, Second);
	Announcement->TimeText->SetText(FText::FromString(CountdownString));
}

void AShooterHUD::UpdateMatchCountDown(int32 CurrentTime)
{
	if (CurrentTime > MatchTime || CurrentTime < 0)
		return;

	int32 Countdown = MatchTime - CurrentTime;

	if (!GetCharacterOverlay() || !GetCharacterOverlay()->MatchCountdown) return;

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

void AShooterHUD::UpdateTopScorePlayer()
{
	const AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	if (!ShooterGameState) return;

	auto PlayerStates = ShooterGameState->GetTopScorePlayerStates();
	if (PlayerStates.IsEmpty()) return;

	if (!CharacterOverlay || !CharacterOverlay->TopScorePlayer) return;

	FString PlayerName;
	for (const auto& State : PlayerStates)
	{
		if (!State) return;
		PlayerName.Append(FString::Printf(TEXT("%s\n"), *State->GetPlayerName()));
	}
	GetCharacterOverlay()->TopScorePlayer->SetText(FText::FromString(PlayerName));
}

void AShooterHUD::UpdateTopScore()
{
	const AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
	if (!ShooterGameState) return;

	const auto PlayerStates = ShooterGameState->GetTopScorePlayerStates();
	const float TopScore = ShooterGameState->GetTopScore();
	if (PlayerStates.IsEmpty()) return;

	if (!GetCharacterOverlay() || !GetCharacterOverlay()->TopScore) return;

	FString TopScoreString;
	for (int32 i = 0; i < PlayerStates.Num(); ++i)
	{
		TopScoreString.Append(FString::Printf(TEXT("%d\n"), FMath::CeilToInt32(TopScore)));
	}
	GetCharacterOverlay()->TopScore->SetText(FText::FromString(TopScoreString));
}

void AShooterHUD::UpdatePlayerScore(float Value)
{
	const FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Value));

	if (!GetCharacterOverlay() || !GetCharacterOverlay()->Score)
		return;

	GetCharacterOverlay()->Score->SetText(FText::FromString(ScoreText));
}

void AShooterHUD::UpdatePlayerDefeats(int32 Value)
{
	const FString DefeatsText = FString::Printf(TEXT("%d"), Value);

	if (!GetCharacterOverlay() || !GetCharacterOverlay()->Defeats)
		return;

	GetCharacterOverlay()->Defeats->SetText(FText::FromString(DefeatsText));
}