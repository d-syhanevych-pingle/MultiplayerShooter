// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/Weapon.h"
#include "Pickups/Pickup.h"
#include "WeaponSpawnerLocation.h"
#include "PickupSpawnerLocation.h"
#include "ItemsSpawner.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AItemsSpawner : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	APawn* GetInstigator() const;

public:	
	void InitializeLocationForSpawn();
	void SpawnRandomItemsOnAllLocations();
	
	UFUNCTION()
	void SpawnRandomWeaponByLocation(AWeaponSpawnerLocation* SpawnerLocation);

	UFUNCTION()
	void SpawnRandomPickupByLocation(APickupSpawnerLocation* SpawnerLocation);

private:

	void SpawnRandomPickupWithDelay(APickupSpawnerLocation* SpawnerLocation, APickup* SpawnedPickup);
	void SpawnRandomWeaponWithDelay(AWeaponSpawnerLocation* SpawnerLocation, AWeapon* SpawnedWeapon);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Spawns", meta = (AllowAbstract = "true"))
	TArray<TSubclassOf<class AWeapon>> WeaponClasses;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Spawns", meta = (AllowAbstract = "true"))
	TArray<TSubclassOf<class APickup>> PickupClasses;

	TArray<AWeaponSpawnerLocation*> WeaponSpawnLocations;
	TArray<APickupSpawnerLocation*> PickupSpawnLocations;

	FDelegateHandle WeaponTakenDelegateHandle;
	FDelegateHandle PickupTakenDelegateHandle;
	FTimerHandle TimerSpawnWeaponHandle;

};
