// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	//initialize character class 
	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}

	if (BlasterCharacter == nullptr) return;

	//getting character speed to implement perform character animation..
	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	//checking if character is in air or not..
	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

	//checking if any movement input getting by player..
	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();

	//initialize value of bIsCrouched with character inherit variable bIsCrouched
	bIsCrouched = BlasterCharacter->bIsCrouched;

	bAiming = BlasterCharacter->IsAiming();

	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	UE_LOG(LogTemp, Warning, TEXT("Aim Rotation: %f"), AimRotation.Yaw);

	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	UE_LOG(LogTemp, Warning, TEXT("Movement Rotation: %f"), MovementRotation.Yaw);

	//getting delta between two rotator which gives value of yaw ofset, which can be used for strafing 
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);

	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);

	YawOffset = DeltaRotation.Yaw;

	

	//functionality for lean
	//getting value difference between character current rotation and character rotation on last frame..
	CharacterRotationLastFrame = CharacterRotation;

	CharacterRotation = BlasterCharacter->GetActorRotation();

	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);

	const float Target = Delta.Yaw / DeltaTime;

	//smooth interpolation for leaning 
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);

	Lean = FMath::Clamp(Interp, -90.f, 90.f);

}
