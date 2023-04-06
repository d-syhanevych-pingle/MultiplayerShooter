// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/UnpredictableBattleGameMode.h"
#include "PlayerController/ShooterPlayerController.h"

void AUnpredictableBattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World)
		return;

	SpawnerEquipment = World->SpawnActor<AEquipmentSpawner>(EquipmentSpawner, FVector::ZeroVector, FRotator::ZeroRotator);
}

void AUnpredictableBattleGameMode::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);

	if (!NewPlayer)
		return;

	if (AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(NewPlayer))
	{
		ShooterPlayerController->OnPawnPosses.AddDynamic(this, &AUnpredictableBattleGameMode::EquipRandomWeaponForCharacter);
	}
}

void AUnpredictableBattleGameMode::EquipRandomWeaponForCharacter(AShooterPlayerController* ShooterPlayerController)
{
	if (!SpawnerEquipment)
		return;

	if (ACharacter* Character = ShooterPlayerController->GetCharacter())
	{
		if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(Character))
		{
			SpawnerEquipment->EquipRandomWeaponForCharacter(MainCharacter);
			UE_LOG(LogTemp, Log, TEXT("EquipRandomWeaponForCharacter"));
		}
	}
}
