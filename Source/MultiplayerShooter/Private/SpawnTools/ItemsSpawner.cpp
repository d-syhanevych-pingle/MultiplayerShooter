// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnTools/ItemsSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameMode.h"

void AItemsSpawner::InitializeLocationForSpawn()
{
	TArray<AActor*> FoundActors;
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::GetAllActorsOfClass(World, AWeaponSpawnerLocation::StaticClass(), FoundActors);

		// Get Weapon Locations
		for (AActor* Actor : FoundActors)
		{
			AWeaponSpawnerLocation* SpawnerLocation = Cast<AWeaponSpawnerLocation>(Actor);
			if (SpawnerLocation)
			{
				WeaponSpawnLocations.Add(SpawnerLocation);
			}
		}


		UGameplayStatics::GetAllActorsOfClass(World, APickupSpawnerLocation::StaticClass(), FoundActors);

		// Get Pickups Locations
		for (AActor* Actor : FoundActors)
		{
			APickupSpawnerLocation* SpawnerLocation = Cast<APickupSpawnerLocation>(Actor);
			if (SpawnerLocation)
			{
				PickupSpawnLocations.Add(SpawnerLocation);
			}
		}
	}
}

// Called when the game starts or when spawned
void AItemsSpawner::BeginPlay()
{
	Super::BeginPlay();
}

APawn* AItemsSpawner::GetInstigator() const
{
	return Cast<APawn>(GetOwner());
}

void AItemsSpawner::SpawnRandomItemsOnAllLocations()
{
	for (AWeaponSpawnerLocation* SpawnerLocation : WeaponSpawnLocations)
	{
		SpawnRandomWeaponByLocation(SpawnerLocation);
	}
	for (APickupSpawnerLocation* SpawnerLocation : PickupSpawnLocations)
	{
		SpawnRandomPickupByLocation(SpawnerLocation);
	}
}

void AItemsSpawner::SpawnRandomWeaponByLocation(AWeaponSpawnerLocation* SpawnerLocation)
{
	UWorld* const World = GetWorld();
	if (!World)
		return;

	if (SpawnerLocation->GetIsExistsWeaponOnLocation())
		return;

	if (WeaponClasses.Num())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		FVector SpawnNewLocation = SpawnerLocation->GetActorLocation();
		FRotator SpawnNewRotation = SpawnerLocation->GetActorRotation();

		int32 RandomClassIndex = FMath::RandRange(0, WeaponClasses.Num() - 1);
		UClass* WeaponClass = WeaponClasses[RandomClassIndex];

		if (WeaponClass)
		{
			AWeapon* SpawnedWeapon = World->SpawnActor<AWeapon>(WeaponClass, SpawnNewLocation, SpawnNewRotation, SpawnParams);
			if (SpawnedWeapon)
			{
				SpawnerLocation->OnWeaponSpawned();

				// Need for respawn weapon on location
				WeaponTakenDelegateHandle = SpawnedWeapon->OnWeaponTakenNative.AddUObject(this, &AItemsSpawner::SpawnRandomWeaponWithDelay, SpawnerLocation, SpawnedWeapon);
			}
		}
	}
}

void AItemsSpawner::SpawnRandomWeaponWithDelay(AWeaponSpawnerLocation* SpawnerLocation, AWeapon* SpawnedWeapon)
{
	UWorld* const World = GetWorld();
	if (!World)
		return;

	if (SpawnedWeapon)
		SpawnedWeapon->OnWeaponTakenNative.Remove(WeaponTakenDelegateHandle);

	SpawnerLocation->OnWeaponTaken();

	int32 RandomTime = FMath::RandRange(5, 20);
	World->GetTimerManager().SetTimer(TimerSpawnWeaponHandle, [this, SpawnerLocation]()
		{
			SpawnRandomWeaponByLocation(SpawnerLocation);
		}, RandomTime, false);
}

void AItemsSpawner::SpawnRandomPickupByLocation(APickupSpawnerLocation* SpawnerLocation)
{
	if (!GetOuter())
		return;
	UWorld* const World = GetWorld();
	if (!World)
		return;

	if (SpawnerLocation->GetIsExistsPickupOnLocation())
		return;

	if (PickupClasses.Num() > 0)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		FVector SpawnNewLocation = SpawnerLocation->GetActorLocation();
		FRotator SpawnNewRotation = SpawnerLocation->GetActorRotation();

		int32 RandomClassIndex = FMath::RandRange(0, PickupClasses.Num() - 1);
		UClass* PickupClass = PickupClasses[RandomClassIndex];

		if (PickupClass)
		{
			APickup* SpawnedPickup = World->SpawnActor<APickup>(PickupClass, SpawnNewLocation, SpawnNewRotation, SpawnParams);
			if (SpawnedPickup)
			{
				SpawnerLocation->OnPickupSpawned();
				// Need for respawn pickup on location
				PickupTakenDelegateHandle = SpawnedPickup->OnPickupTakenNative.AddUObject(this, &AItemsSpawner::SpawnRandomPickupWithDelay, SpawnerLocation, SpawnedPickup);

			}
		}
	}
}

void AItemsSpawner::SpawnRandomPickupWithDelay(APickupSpawnerLocation* SpawnerLocation, APickup* SpawnedPickup)
{
	UWorld* const World = GetWorld();
	if (!World)
		return;

	if (SpawnedPickup)
		SpawnedPickup->OnPickupTakenNative.Remove(PickupTakenDelegateHandle);

	SpawnerLocation->OnPickupTaken();

	FTimerHandle TimerHandle;
	int32 RandomTime = FMath::RandRange(5, 20);
	World->GetTimerManager().SetTimer(TimerHandle, [this, SpawnerLocation]()
		{
			SpawnRandomPickupByLocation(SpawnerLocation);
		}, RandomTime, false);
}


