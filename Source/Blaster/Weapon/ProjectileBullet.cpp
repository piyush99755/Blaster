// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"


AProjectileBullet::AProjectileBullet()
{
	//constructing projectile movement component...
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//casting character first as it through character accessing controller which is required parameter for apply damange function ...
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;

		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
		}
	}
    //calling super::OnHit at the end just becuase it destroy bullet, that should not happen before apply damage 
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
