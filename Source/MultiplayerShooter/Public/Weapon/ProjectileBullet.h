// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AProjectileBullet : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileBullet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	/** Apply Damage when on hit */
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	/** We customize the ProjectileMovementComponent to override the UProjectileMovementComponent engine provides. */
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=ShooterMovementComponent_OnRep)
	class UShooterMovementComponent* ShooterMovementComponent;

	UFUNCTION()
	void ShooterMovementComponent_OnRep();
};
