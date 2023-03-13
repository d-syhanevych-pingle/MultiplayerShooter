// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "WeaponProjectile.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MULTIPLAYERSHOOTER_API AWeaponProjectile : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& TraceHitTarget) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay();
private:
	void FireProjectile(const FVector& TraceHitTarget);
	void EjectProjectileShell();

	FRandomStream RandomStream;

	UPROPERTY(ReplicatedUsing=Seed_OnRep)
	int32 Seed;

	UFUNCTION()
	void Seed_OnRep();
};
