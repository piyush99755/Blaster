// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystem.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Blaster.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "NiagaraComponent.h"

AProjectile::AProjectile()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	 //this makes projectile replicates to clients as well...
	bReplicates = true; 

	//construct collision box for projectile and setting up its collision properties..
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

	



}


void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	//spawn emitter attached to the projectile, which results in glowing trail while travelling in world...
	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(Tracer, CollisionBox, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}


	//Only handles on server....
	if (HasAuthority())
	{
		//bind call back to component hit event...
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
	
	
}




void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

void AProjectile::SpawnTrailSystem()
{
	
		if (TrailSystem)
		{
			//spawning trail system 
			TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				TrailSystem,
				GetRootComponent(),
				FName(),
				GetActorLocation(),
				GetActorRotation(),
				EAttachLocation::KeepWorldPosition,
				false);
		}
	
}

void AProjectile::ExplodeDamage()
{
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn)
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,
				Damage,
				10.f,
				GetActorLocation(),
				200.f,
				500.f,
				1.f,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				this,
				FiringController)
				;
		}
	}
}

//this is an override function, which is being called when actor is explicitly being destroy during gameplay..
//thus, spawning particels and play sound will be  propogated to server and all clients as well
void AProjectile::Destroyed()
{
	Super::Destroyed();


	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

}

void AProjectile::StartDestroyTimer()
{
	//setting timer to delay destroying projectile.. 
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AProjectile::DestroyTimeFinished, DestroyTime);
}

void AProjectile::DestroyTimeFinished()
{
	Destroy();
}

