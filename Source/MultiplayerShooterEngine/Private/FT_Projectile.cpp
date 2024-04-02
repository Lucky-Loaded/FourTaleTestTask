// Fill out your copyright notice in the Description page of Project Settings.


#include "FT_Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/PlayerState.h"

class UMaterialInterface;
class ProjectileMovementComponent;

// Sets default values
AFT_Projectile::AFT_Projectile()
{
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>("Sphere");
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	Sphere->SetupAttachment(RootComponent);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AFT_Projectile::OnSphereOverlap);
	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AFT_Projectile::OnBounce);

	PrimaryActorTick.bCanEverTick = true;

}

void AFT_Projectile::OnConstruction(const FTransform& Transform)
{
	SetActorScale3D(FVector(SphereScale));
}

// Called when the game starts or when spawned
void AFT_Projectile::BeginPlay()
{
	Super::BeginPlay();

	if(!IsValid(GetInstigator())) return;

	APawn* lInstigator = GetInstigator();
	APlayerState* lPlayerState = lInstigator->GetPlayerState();

	OldLocation = GetActorLocation();
	SetActorHiddenInGame(lInstigator->IsLocallyControlled() != IsLocal);

	if (!IsLocal && IsValid(lPlayerState))
		AddActorWorldOffset(ProjectileMovementComponent->InitialSpeed * lPlayerState->GetPingInMilliseconds() / 2000.f * GetActorRotation().Vector(), true);
}

// Called every frame
void AFT_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FHitResult HitResult;
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, OldLocation, GetActorLocation(), ECollisionChannel::ECC_Visibility)) {
		if (HitResult.GetActor() != GetInstigator())
		{
			OnHitedBind.Broadcast(HitResult);
			DestroyProjectile();
		}
	}

	OldLocation = GetActorLocation();
}

void AFT_Projectile::DestroySelf()
{
	Destroy();
}

void AFT_Projectile::DestroyProjectile()
{
	ProjectileMovementComponent->StopMovementImmediately();
	ProjectileMovementComponent->SetComponentTickEnabled(false);
	Sphere->SetVisibility(false);

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AFT_Projectile::DestroySelf, 1.f, false);
}

void AFT_Projectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator()) return;

	OnHitedBind.Broadcast(SweepResult);
	DestroyProjectile();
}

void AFT_Projectile::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (!IsLocal) OnHitedBind.Broadcast(ImpactResult);

	DestroyProjectile();
}

