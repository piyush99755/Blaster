// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"

// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->TargetArmLength = 600.f;
	SpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Construct overhead widget..
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetRootComponent());


	bUseControllerRotationYaw = false;
	//character orient towards movement
	GetCharacterMovement()->bOrientRotationToMovement = true;


}

//this function is mainly used to register replicated variables...
void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//this function is used to relicate variable,which takes class and variable and condition as parameters..
	//this will replicate variable to owner only, means one who overlapping with weapon
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}



// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}



// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("Lookup", this, &ABlasterCharacter::Lookup);

}



void ABlasterCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction * Value);
	}
}

void ABlasterCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction * Value);
	}
}

void ABlasterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABlasterCharacter::Lookup(float Value)
{
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	//rep notifies only replicate variables to client
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	//this will set value to false, when client accessed nullptr
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;

	//this will display widget for server as well.
	//checking if its locally controlled by server 
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
		
	}

}

