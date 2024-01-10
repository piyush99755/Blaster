// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AShotgun::Fire(const FVector& HitTarget)
{
	
	AWeapon::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();


	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		//getting socket transform, also start and end location which will be required in line tracing
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		uint32 Hits = 0;

		TMap<ABlasterCharacter*, uint32>HitMap;

		for (uint32 i = 0; i < NumberOfPelltes; i++)
		{
			FHitResult FireHit; 
			WeaponTraceHit(Start, HitTarget, FireHit);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter && HasAuthority() && InstigatorController)
			{
				if (HitMap.Contains(BlasterCharacter))
				{
					//if HitMap contains key of Blaster character then increment hits ..
					HitMap[BlasterCharacter]++;
				}
				else
				{
					HitMap.Emplace(BlasterCharacter, 1);
				}
				
			}

			if (ImpactParticles)
			{
				//spawn particles where line tracing is hitting 
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
					ImpactParticles,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation());
			}

			if (HitSound)
			{
				//play sound at location of impact point of line tracing
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, FireHit.ImpactPoint, 0.5f, FMath::FRandRange(-.5f , .5f));
			}
		}

		for (auto HitPair : HitMap)
		{
			if (HitPair.Key && HasAuthority() && InstigatorController)
			{
				//apply damage when its a blaster character.. 
				UGameplayStatics::ApplyDamage(
					HitPair.Key,
					Damage * HitPair.Value,
					InstigatorController,
					this,
					UDamageType::StaticClass());

			}
		}
		
		


	}
}