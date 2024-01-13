// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlasterAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Blaster/Blaster.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/Gamemode/BlasterGameMode.h"
#include "TimerManager.h"
#include "Blaster/Weapon/WeaponTypes.h"



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

	//constructing combat component..
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	//actor component class does not need to register variable for replication.. they itself are replicated
	CombatComponent->SetIsReplicated(true);


	bUseControllerRotationYaw = false;
	//character orient towards movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	//setting value to true by default
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	//prevent character from blocking camera ...
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 650.f);

	//initial state of TurningInPlace
	TurningInPlace = ETurningInPlace::ETIP_NoTurning;
	

	//net frequency variables for fast-paced shooter game...
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

//allow actor to initilize them before all of its components....
void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CombatComponent)
	{
		
		CombatComponent->Character = this;
	}
	
}




//this function is mainly used to register replicated variables...
void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//this function is used to relicate variable,which takes class and variable and condition as parameters..
	//this will replicate variable to owner only, means one who overlapping with weapon
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
}



// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	
	UpdateHUDHealth();
	
	if (HasAuthority())
	{
		

		//binding call back 
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}
	
	
}



// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCameraIfClose();

}

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NoTurning;
		return;
	}

	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);// calling every frame 
	}
	else
	{
		//after certain amount of time , if movement isnt replicated , then calling OnRep_ReplicatedMovement function ...
		TimeSinceLastReplicatedMovement += DeltaTime;
		if (TimeSinceLastReplicatedMovement > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}

		CalculateAO_Pitch();
	}
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (CombatComponent && CombatComponent->EquippedWeapon == nullptr) return;

	
	float Speed = CalculateSpeed();

	float bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still or not jumping
	{
		bRotateRootBone = true;
		//getting aim rotation yaw by getting difference between two rotators which are current aim rotation and starting aim rotation
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation );
		AO_Yaw = DeltaAimRotation.Yaw;

		if (TurningInPlace == ETurningInPlace::ETIP_NoTurning)
		{
			 InterpAO_Yaw  = AO_Yaw;
		}

		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}

	if (Speed > 0.f || bIsInAir)//running or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NoTurning;//not turning if speedin >0.f or character is jumping..
	}

	CalculateAO_Pitch();
}

void ABlasterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;

	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}



void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}	

	else if (AO_Yaw < -90.F)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	//for smooth turning in place interpolation...
	if (TurningInPlace != ETurningInPlace::ETIP_NoTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 5.f);
		AO_Yaw = InterpAO_Yaw;

		//prevent over turning
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NoTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);//resetting starting aim rotation
		}

	}
}



// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("Lookup", this, &ABlasterCharacter::Lookup);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABlasterCharacter::ReloadButtonPressed);
	

}





void ABlasterCharacter::MoveForward(float Value)
{
	if (bDisableGameplay) return;

	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction * Value);
	}
}

void ABlasterCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;

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

void ABlasterCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;
	
	if (CombatComponent)
	{
		//if equip button pressed by server , simply call equipweapon function 
		if (HasAuthority())
		{
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			//when equip button pressed by client, call RPC function
			ServerEquipButtonPressed();
		}
		
	}
}



void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (CombatComponent)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
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

void ABlasterCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	PlayHitReactMontage();
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHealthHUD(Health, MaxHealth);
	}
}

void ABlasterCharacter::Elimination()
{
	if (CombatComponent && CombatComponent->EquippedWeapon)
	{
		CombatComponent->EquippedWeapon->Dropped();
	}
	MulticastElimination();

	GetWorldTimerManager().SetTimer(ElimTimerHandle,this,  &ABlasterCharacter::ElimTimerFinished, ElimDelay);

}



void ABlasterCharacter::MulticastElimination_Implementation()
{
	PlayDeathMontage();


	//disable character movement..
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	//DeathFinished();
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetWeaponAmmoHUD(0);
	}

	bEliminated = true;
	

	

	bDisableGameplay = true;

	//disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	

	

	

	//destroy weapon on elimination
	if (CombatComponent && CombatComponent->EquippedWeapon)
	{
		//CombatComponent->EquippedWeapon->Destroy();
	}
    
	//hiding sniper score widget on elimination...
	bool bHideSniperScope = IsLocallyControlled() 
		&& CombatComponent 
		&&CombatComponent->bAiming 
		&& CombatComponent->EquippedWeapon 
		&& GetEquippedWeapon()->GetWeaponType() == EWeaponType::EWT_SniperRifle;

	if (bHideSniperScope)
	{
		ShowSniperScoreWidget(false);
	}
}



void ABlasterCharacter::ElimTimerFinished()
{
	//once elim timer finished then this function from game mode will respawn player at ramdom player start location 
	ABlasterGameMode* BlasterGameMode = Cast <ABlasterGameMode>(GetWorld()->GetAuthGameMode());
	if (BlasterGameMode)
	{
		BlasterGameMode->RequestRespawn(this, Controller);
		
	}
}

void ABlasterCharacter::DeathFinished()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	bEliminated = true;

	if (BlasterPlayerController)
	{
		DisableInput(BlasterPlayerController);
	}

	
	//disable character movement..
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	

	bDisableGameplay = true;

	//disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (DamagedActor == this)
	{
		Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);


		UpdateHUDHealth();
		PlayHitReactMontage();

		if (Health == 0.f)
		{


			//casting blaster game mode... 
			ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(GetWorld()->GetAuthGameMode());
			if (BlasterGameMode)
			{
				Elimination();
				BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;

				ABlasterPlayerController* AttackingController =  Cast<ABlasterPlayerController>(InstigatedBy->GetInstigatorController());
				BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackingController);
			}



		}



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

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (CombatComponent && CombatComponent->EquippedWeapon);
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		//it is inherited function from Character class
		UnCrouch();
	}
	else
	{
		Crouch();
	}

	
}

void ABlasterCharacter::AimButtonPressed()
{
	if (bDisableGameplay) return;

	if (CombatComponent)
	{
		CombatComponent->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if (bDisableGameplay) return;

	if (CombatComponent)
	{
		CombatComponent->SetAiming(false);
	}
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;

	if (CombatComponent)
	{
		CombatComponent->Reload();
	}
}



void ABlasterCharacter::FireButtonPressed()
{
	if (bDisableGameplay) return;

	if (CombatComponent)
	{
		CombatComponent->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if (bDisableGameplay) return;

	if (CombatComponent)
	{
		CombatComponent->FireButtonPressed(false);
	}
}




bool ABlasterCharacter::IsAiming()
{
	return (CombatComponent && CombatComponent->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if(CombatComponent == nullptr) return nullptr;

	return CombatComponent->EquippedWeapon;
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	if (CombatComponent == nullptr) return ECombatState::ECS_MAX;

	return CombatComponent->CombatState;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (CombatComponent == nullptr) return FVector();

	return CombatComponent->HitTarget;
}

//overriding jump base function to correct crouching behavior
void ABlasterCharacter::Jump()
{
	if (bDisableGameplay) return; 

	if (bIsCrouched)
	{
		UnCrouch();
	}

	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::PlayFireWeaponMontage(bool bAiming)
{
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && WeaponFireMontage)
	{
		
		AnimInstance->Montage_Play(WeaponFireMontage);
		FName SectionName;
		SectionName = bAiming ? FName("AimRifle") : FName("HipRifle");//getting section name to play appropriate montage
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (CombatComponent->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("RifleReload");
			break;

		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketReload");
			break;

		case EWeaponType::EWT_Pistol:
			SectionName = FName("PistolReload");
			break;

		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("SMGReload");
			break;

		case EWeaponType::EWT_Shotgun:
			SectionName = FName("ShotgunReload");
			break;

		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("RifleReload");
			break;

		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("RifleReload");
			break;
		}
        AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{

		AnimInstance->Montage_Play(HitReactMontage);
		
		FName SectionName("FromFront");//getting section name to play appropriate montage
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayDeathMontage()
{
	

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(FName("Elim"));
    }
}

void ABlasterCharacter::HideCameraIfClose()
{
	if (!IsLocallyControlled()) return;

	if ((GetFollowCamera()->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		//hiding character mesh..
		GetMesh()->SetVisibility(false);

		//hiding weapon and its mesh..
		if (CombatComponent && CombatComponent->EquippedWeapon && CombatComponent->EquippedWeapon->GetWeaponMesh())
		{
			CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}

	else
	{
		GetMesh()->SetVisibility(true);
		if (CombatComponent && CombatComponent->EquippedWeapon && CombatComponent->EquippedWeapon->GetWeaponMesh())
		{
			CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}

	}
}

void ABlasterCharacter::SimulatedProxiesTurn()
{
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;
	
	float Speed = CalculateSpeed();
	
	bRotateRootBone = false; 

	//to prevent character sliding while speed is more than zero..
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NoTurning;
		return;
	}

	
	//setting up turning in place functionality for simulated proxy...
	SimulatedProxyRotationLastFrame = SimulatedProxyRotation;
	SimulatedProxyRotation = GetActorRotation();

	SimulatedProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(SimulatedProxyRotation, SimulatedProxyRotationLastFrame).Yaw;

	if (FMath::Abs(SimulatedProxyYaw) > TurnThreshold)
	{
		if (SimulatedProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (SimulatedProxyYaw > -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NoTurning;
		}
		return;
		
	}
	
	TurningInPlace = ETurningInPlace::ETIP_NoTurning;
}

//used for replication of our root component's position and velocity..
void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	
		SimulatedProxiesTurn();
	

	TimeSinceLastReplicatedMovement = 0.f;
}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
	 
}





