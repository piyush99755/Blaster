// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponTypes.h"

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

		FHitResult FireHit;

		WeaponTraceHit(Start, HitTarget, FireHit);

		if (FireHit.bBlockingHit)
		{


			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter && HasAuthority() && InstigatorController)
			{
				//apply damage when its a blaster character.. 
				UGameplayStatics::ApplyDamage(
					BlasterCharacter,
					Damage,
					InstigatorController,
					this,
					UDamageType::StaticClass());

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
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, FireHit.ImpactPoint);
			}

		}



		//Spawn particles and play sound at location of muzxzle flash socket...
		if (FireParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticles, GetActorLocation());
		}

		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());
		}



	};
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	//getting length of vector.. 
	FVector DistanceToNormalize = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + DistanceToNormalize * DistaceToSphere;

	//random vector of sphere
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);

	//end location of trace hit
	FVector EndLoc = SphereCenter + RandVec; //end location of trace hit
    FVector ToEndLoc = EndLoc - TraceStart; 

	/*
	* DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	true)
	DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Green, true);
	DrawDebugLine(GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
		FColor::Yellow,
	;*/

	//to divide by ToEndLoc magnitude to prevent getting overflow values of x, y and z vector
	return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	
	UWorld* World = GetWorld();

	//if use scatter then trace hit differently for shot gun or just trace hit normally.. .
	FVector End = bUseScatter? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;

	if (World)
	{
		World->LineTraceSingleByChannel(OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility);
	}
	FVector BeamEnd = End;

	if (OutHit.bBlockingHit)
	{
		BeamEnd = OutHit.ImpactPoint;

		if (BeamParticles)
		{
			//storing particle system in a particle system component..
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World,
				BeamParticles,
				TraceStart,
				FRotator::ZeroRotator,
				true);

			if (Beam)
			{
				//Target is a parameter value mentioned in Beam particle system , BeamEnd is a Target for beam particles
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}

		}
	}
}
