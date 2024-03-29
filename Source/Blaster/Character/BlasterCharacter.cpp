// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlasterAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Blaster/Blaster.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/Gamemode/BlasterGameMode.h"
#include "TimerManager.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"




ABlasterCharacter::ABlasterCharacter()
{
 	
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

	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("Buff Component"));
	BuffComponent->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));


	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);


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

	//hit boxes for server-side rewind

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("head"), head);

	Pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT(" Pelvis"));
	Pelvis->SetupAttachment(GetMesh(), FName(" Pelvis"));
	Pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("Pelvis"), 	Pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("spine_03"), spine_03);


	UpperArm_L = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperArm_L"));
	UpperArm_L->SetupAttachment(GetMesh(), FName("UpperArm_L"));
	UpperArm_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("UpperArm_L"), UpperArm_L);

	UpperArm_R = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperArm_R"));
	UpperArm_R->SetupAttachment(GetMesh(), FName("UpperArm_R"));
	UpperArm_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("UpperArm_R"), UpperArm_R);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	lowerarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	lowerarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("lowerarm_r"), lowerarm_r);

	Hand_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Hand_L"));
	Hand_L->SetupAttachment(GetMesh(), FName("Hand_L"));
	Hand_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("Hand_L"), Hand_L);

	Hand_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Hand_R"));
	Hand_R->SetupAttachment(GetMesh(), FName("Hand_R"));
	Hand_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("Hand_R"), Hand_R);

	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), FName("backpack"));
	backpack->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("backpack"), backpack);

	Thigh_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Thigh_L"));
	Thigh_L->SetupAttachment(GetMesh(), FName("Thigh_L"));
	Thigh_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("Thigh_L"), Thigh_L);

	Thigh_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Thigh_R"));
	Thigh_R->SetupAttachment(GetMesh(), FName("Thigh_R"));
	Thigh_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("Thigh_R"), Thigh_R);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	calf_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	calf_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("calf_r"), calf_r);

	Foot_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Foot_L"));
	Foot_L->SetupAttachment(GetMesh(), FName("Foot_L"));
	Foot_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("Foot_L"), Foot_L);

	Foot_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Foot_R"));
	Foot_R->SetupAttachment(GetMesh(), FName("Foot_R"));
	Foot_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollisions.Add(FName("Foot_R"), Foot_R);



}

//allow actor to initilize them before all of its components....
void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CombatComponent)
	{
		
		CombatComponent->Character = this;
	}

	if (BuffComponent)
	{
		BuffComponent->Character = this;

		//Initialize speeds variables with character movement component variables
		BuffComponent->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);

		BuffComponent->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	
	if (LagCompensation)
	{
		LagCompensation->Character = this;

		if (Controller)
		{
			LagCompensation->Controller = Cast<ABlasterPlayerController>(Controller);
		}
		
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

	SpawnDefaultWeapon();

	UpdateAmmoHUD();
	
	UpdateHUDHealth();
	
	if (HasAuthority())
	{
	    //binding call back 
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
		UpdateHUDHealth();
	}
	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
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

	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &ABlasterCharacter::ThrowGrenadeButtonPressed);
	

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
		//calling server RPC which works on both server and client...
		ServerEquipButtonPressed();
		
	}
}



void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (CombatComponent)
	{
		//when weapon is still avaialable to equip and hasvalue of overlapping weapon, calling equip weapon()
		if (OverlappingWeapon)
		{
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		//when both weapons are attached to the charactermesh, calling swap weapons on press E key.
		else if (CombatComponent->ShouldSwapWeapons())
		{
			CombatComponent->SwapWeapons();
		}
		
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

void ABlasterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();

	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
	
}



void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHealthHUD(Health, MaxHealth);
	}
}

void ABlasterCharacter::UpdateAmmoHUD()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController && CombatComponent && CombatComponent->EquippedWeapon)
	{
		BlasterPlayerController->SetWeaponAmmoHUD(CombatComponent->CarriedAmmo);
		BlasterPlayerController->SetCarriedAmmoHUD(CombatComponent->EquippedWeapon->GetAmmo());
	}
}



void ABlasterCharacter::Elimination()
{
	MulticastElimination();
	DropOrDestroyWeapons();
	

	GetWorldTimerManager().SetTimer(ElimTimerHandle,this,  &ABlasterCharacter::ElimTimerFinished, ElimDelay);

}




void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	//drop or destroy weapon on elimination of player
	if (Weapon == nullptr) return;
	{
		if (Weapon->bWeaponDestroy)
		{
			Weapon->Destroy();
		}
		else
		{
			Weapon->Dropped();
		}
	}
}

void ABlasterCharacter::DropOrDestroyWeapons()
{
	if (CombatComponent)
	{
		//if its primary weapon same as default spawn weapon , destroy it 
		//if its secondary weapon , drop it
		if (CombatComponent->EquippedWeapon)
		{
			DropOrDestroyWeapon(CombatComponent->EquippedWeapon);
		}
		if (CombatComponent->EquippedSecondaryWeapon)
		{
			DropOrDestroyWeapon(CombatComponent->EquippedSecondaryWeapon);

		}
	}

}



void ABlasterCharacter::MulticastElimination_Implementation()
{
	PlayDeathMontage();
	bEliminated = true;
    bDisableGameplay = true;


	//disable character movement..
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	//DeathFinished();
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetWeaponAmmoHUD(0);
	}

	

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

void ABlasterCharacter::SpawnDefaultWeapon()
{
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (BlasterGameMode && World && DefaultWeaponClass && !bEliminated)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bWeaponDestroy = true;
		if (CombatComponent)
		{
			CombatComponent->EquipWeapon(StartingWeapon);
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

void ABlasterCharacter::ThrowGrenadeButtonPressed()
{
	if (CombatComponent)
	{
		CombatComponent->ThrowGrenade();
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

bool ABlasterCharacter::IsLocallyReloading()
{
	if(CombatComponent == nullptr) return false;

	return CombatComponent->bLocallyReloading;
	
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

void ABlasterCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
		
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





