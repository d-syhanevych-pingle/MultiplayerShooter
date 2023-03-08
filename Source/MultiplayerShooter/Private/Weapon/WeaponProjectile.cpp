// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponProjectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/Casing.h"
#include "Weapon/Projectile.h"
#include "Net/UnrealNetwork.h"

void AWeaponProjectile::Fire(const FVector& TraceHitTarget)
{
	// Common game logic (weapon functionality)
	// Call the code in the parent's function. It's something like clone the parent's code here.
	Super::Fire(TraceHitTarget);
	
	// Respective game logic (weapon functionality)
	if (HasAuthority())
	{
		FireProjectile(TraceHitTarget);
	}
	
	//It is performed both on the server and on the client (so as not to load the network)
	EjectProjectileShell();
}

void AWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		RandomStream.GenerateNewSeed();
		Seed = RandomStream.GetCurrentSeed();
	}
	else
	{
		RandomStream.Initialize(Seed);
	}
}

void AWeaponProjectile::ServerFireProjectile_Implementation(const FVector& TraceHitTarget)
{
	FireProjectile(TraceHitTarget);
}

void AWeaponProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponProjectile, Seed);
}

void AWeaponProjectile::Seed_OnRep()
{
	RandomStream.Initialize(Seed);
}

void AWeaponProjectile::FireProjectile(const FVector& TraceHitTarget)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector ToTargetDirection = TraceHitTarget - SocketTransform.GetLocation();
		const FRotator TargetRotation = ToTargetDirection.Rotation();
		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			// Set weapon's owner in UCombatComponent::EquipWeapon(), and the owner is the character. Set Owner for later use.
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				// Spawn a child object (ProjectileClass* ) in the world and return the base object reference (AProjectile* )
				World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams
					);
			}
		}
	}
}

void AWeaponProjectile::EjectProjectileShell()
{
	const USkeletalMeshSocket* AmmoEjectSocket = GetWeaponMesh()->GetSocketByName(FName("AmmoEject"));
	if (AmmoEjectSocket)
	{
		const FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());
		if (CasingClass)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				// Spawn a child object (ProjectileClass* ) in the world and return the base object reference (AProjectile* )
				ACasing* Casing = World->SpawnActor<ACasing>(
						CasingClass,
						SocketTransform.GetLocation(),
						SocketTransform.GetRotation().Rotator(),
						FActorSpawnParameters()
						);

				Casing->AddImpulseRandomly(RandomStream);
				Seed = RandomStream.GetCurrentSeed();
			}
		}
	}
}

