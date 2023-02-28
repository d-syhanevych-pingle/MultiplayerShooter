// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FindAndShootGameMode.h"


void AFindAndShootGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World)
		return;

	SpawnerItems = World->SpawnActor<AItemsSpawner>(ItemsSpawner, FVector::ZeroVector, FRotator::ZeroRotator);
	SpawnerItems->InitializeLocationForSpawn();
	SpawnerItems->SpawnRandomItemsOnAllLocations();
}