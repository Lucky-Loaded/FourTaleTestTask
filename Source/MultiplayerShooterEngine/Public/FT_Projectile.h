// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FT_Projectile.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHited, const FHitResult&, Hit);

UCLASS(Abstract)
class MULTIPLAYERSHOOTERENGINE_API AFT_Projectile : public AActor
{
	GENERATED_BODY()

//Constructor
public:

	AFT_Projectile();

//C++ protected methods
protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

//C++ public methods
public:	

	virtual void Tick(float DeltaTime) override;

//C++ private methods
private:
	
	UFUNCTION()
	void DestroySelf();
	UFUNCTION()
	void DestroyProjectile();


	UFUNCTION()
		virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	UPROPERTY(BlueprintAssignable, Category = "BaseProjectile|EventsForBind")
		FOnHited OnHitedBind;

//C++ values
private:

	FTimerHandle  DestroyTimer;
	FVector OldLocation = FVector::ZeroVector;
	bool IsLocal = false;

//Blueprint values
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseProjectile")
		class UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseProjectile")
		UStaticMeshComponent* Sphere= nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseProjectile")
		float SphereScale = 1.f;
};
