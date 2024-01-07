// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();


	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		//getting socket transform, also start and end location which will be required in line tracing
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget - Start) * 1.25f;
		FHitResult FireHit;



		UWorld* World = GetWorld();
		if (World)
		{
			World->LineTraceSingleByChannel(FireHit,
				Start,
				End,
				ECollisionChannel::ECC_Visibility);

			FVector BeamEnd = End; 

			if (FireHit.bBlockingHit)
			{
				BeamEnd = FireHit.ImpactPoint;

				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
				if (BlasterCharacter && HasAuthority() && InstigatorController)
				{
					
						UGameplayStatics::ApplyDamage(
							BlasterCharacter,
							Damage,
							InstigatorController,
							this,
							UDamageType::StaticClass());

				}



				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(World,
						ImpactParticles,
						FireHit.ImpactPoint,
						FireHit.ImpactNormal.Rotation());
				}

			}

			if (BeamParticles)
			{
				//storing particle system in a particle system component..
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World,
					BeamParticles,
					SocketTransform);

				if (Beam)
				{
					//Target is a parameter value mentioned in Beam particle system , BeamEnd is a Target for beam particles
					Beam->SetVectorParameter(FName("Target"), BeamEnd);
				}

			}
		};


	}
}
