// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileBullet.h"
#include "Weapon/ShooterMovementComponent.h"
#include "Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AProjectileBullet::AProjectileBullet()
{
	ShooterMovementComponent = CreateDefaultSubobject<UShooterMovementComponent>(TEXT("Bullet Movement Component"));
	ShooterMovementComponent->bRotationFollowsVelocity = true;
	ShooterMovementComponent->InitialSpeed = 15000.f;
	ShooterMovementComponent->MaxSpeed = 15000.f;
	//ShooterMovementComponent->SetIsReplicated(true);
}

void AProjectileBullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectileBullet, ShooterMovementComponent);
}

void AProjectileBullet::ShooterMovementComponent_OnRep()
{
	// Get a reference to the ProjectileMovementComponent
	UProjectileMovementComponent* ProjectileMovement = ShooterMovementComponent;

	// Save the starting location of the projectile
	FVector StartLocation = GetActorLocation();

	// Loop over the specified number of frames
	float Distance = 5000.f; // Maximum distance to print the flight path for
	float Velocity = ProjectileMovement->Velocity.Size(); // Current velocity of the projectile
	float DeltaTime = GetWorld()->GetDeltaSeconds(); // Time between frames
	int NumFrames = FMath::CeilToInt(Distance / (Velocity * DeltaTime));
	for (int FrameNum = 0; FrameNum < NumFrames; FrameNum++)
	{
		// Get the current location of the projectile
		FVector EndLocation = GetActorLocation();

		// Draw a line between the previous location and the current location
		if (FrameNum > 0)
		{
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1.f);
		}

		// Update the position of the projectile
		ProjectileMovement->TickComponent(DeltaTime, ELevelTick::LEVELTICK_All, NULL);

		// Save the current location as the previous location for the next frame
		StartLocation = EndLocation;
	}
}
void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Damage event will be caught only on the server
	if (HasAuthority())
	{
		// The Owner/Instigator is set in SpawnParams when we spawn the projectile
		const APawn* ProjectileInstigator = GetInstigator();
		if (!ProjectileInstigator) return;

		// If we hit ourselves, it'll not trigger the HitImpact.
		if (OtherActor == GetOwner()) return;

		// ApplyDamage logic
		UGameplayStatics::ApplyDamage(OtherActor, Damage, ProjectileInstigator->GetController(), this, UDamageType::StaticClass());
	}

	// Destroy() will be called, so Super::OnHit should be called at last.
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	
}

