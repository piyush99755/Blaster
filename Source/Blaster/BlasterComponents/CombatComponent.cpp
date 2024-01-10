// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 250.f;

	
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	//set it to base walk speed, while not aiming..
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}

		if (Character->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
	
	}

	
	
}
void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);//replicating equipped weapon variable so it reflect to all clients
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	

	//as it only meaningfull to autonomous proxy.. 
	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);

		InterpFOV(DeltaTime);
	}
	

}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;

	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;

		if (HUD)
		{
			if (EquippedWeapon)
			{
				//intializing crosshairs info with equipped weapon crosshairs 
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			}

			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}


			//calculating crosshair spread... 
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			//clamp walk speed range which is (0 to 600) into Velocity multiplier range whichis 0 to 1..
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			//when character is in air..
			if (Character->GetCharacterMovement()->IsFalling())
			{
				//Spread when jumping 
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}

			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				//shrink when aiming
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 20.f);

			//adding crosshair factor while spreading and substracting crosshair factor while shrinking..
			HUDPackage.CrosshairSpread = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;

			HUD->SetHUDPackage(HUDPackage);
			

		}

		
	}

}

bool UCombatComponent::CanFire()
{
	if(EquippedWeapon == nullptr) return false;
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
	
}



void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bAiming);

	if (Character)
	{
		//updating AimWalkSpeed on client side...
		Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;

	if (Character)
	{
		//updating AimWalkSpeed on server side...
		Character->GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	//replicated these values to other clients as well
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponStatus(EWeaponState::EWS_Equipped);
        const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
        if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;

		if (EquippedWeapon->GetEquipSound())
		{
			UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->GetEquipSound(), Character->GetActorLocation());
		}
	}
}








void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;

	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}

	EquippedWeapon = WeaponToEquip;

	

	//setting weapon status on equipping weapon 
	EquippedWeapon->SetWeaponStatus(EWeaponState::EWS_Equipped);

	//getting hand socket added in character skeletal mesh for weapon
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (HandSocket)
	{
		//attaching weapon to hand socket
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	//setting weapon owner as character
	EquippedWeapon->SetOwner(Character);

	//initialize Carried ammo map key (weapon type)before setting up carried ammo value to the HUD..
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller; 
	if (Controller)
	{
        Controller->SetCarriedAmmoHUD(CarriedAmmo);
	}
	EquippedWeapon->SetHUDAmmo();

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;

	//play sound on equipping weapon
	if (EquippedWeapon->GetEquipSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->GetEquipSound(), Character->GetActorLocation());
	}

	//automatically reload weapon when its empty
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}


	
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState != ECombatState::ECS_Reloading)
	{
		ServerReload();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr) return;

	CombatState = ECombatState::ECS_Reloading;
	HandleReload();

}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;

	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			Fire();
			
		}
		break;
	}
}

void UCombatComponent::HandleReload()
{
	Character->PlayReloadMontage();
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;
	//calculating room in magazine...
	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();
	
	//with right type of weapon , getting number of ammocarried, getting min value between room in magazine and amount carried..
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);//clamping value to make sure its doesnot go below 0
	}
	
	return 0;
}

void UCombatComponent::UpdateAmmoValues()
{
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		//deducting reload amount from actual carried ammo 
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;

		//updating carried ammo corresponding to weapon type..
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];

	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetCarriedAmmoHUD(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(-ReloadAmount);
}

void UCombatComponent::InitializeCarriedAmmo()
{
	//emplace let us avoid storing any temporary values..
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SubmachineGun, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgunAmmo);

}





void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if (bFireButtonPressed && EquippedWeapon)
	{
		Fire();
	}

	
}

void UCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		//as FireButtonPressed function called locally on server or client
	//need to call server RPC first and it will call multicast RPC 
		ServerFire(HitTarget);

		if (EquippedWeapon)
		{
			CrosshairShootingFactor = 5.f;
		}

		FireTimerStart();
	}
	
	
	
}

void UCombatComponent::FireTimerStart()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;

	//set timer for fire 
	Character->GetWorldTimerManager().SetTimer(FireHandle, this, &UCombatComponent::FireTimeFinished, EquippedWeapon->FireDelay);
}

void UCombatComponent::FireTimeFinished()
{
	if (EquippedWeapon == nullptr) return; 
	
	bCanFire = true;

	if (bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
	//reload weaponwhen its empty..
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetCarriedAmmoHUD(CarriedAmmo);
	}
}



void UCombatComponent::TraceUnderCrosshairs(FHitResult& HitResult)
{
	//get the viewport size
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//gettinng cross hair location by dividing viewport zise x and y vector by 2.f..
	FVector2D CrosshairLocation = FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	//project screen to world location
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		//variables for line trace single by channel
		FVector Start = CrosshairWorldPosition;
		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
			
		}
		 
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if (!HitResult.bBlockingHit)
		{
			HitResult.ImpactPoint = End;
			
		}

		//Actor who implements interface while line tracing ,crosshair color changes to red..
		if (HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractWithCrosshairInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}
		
	};

	

	

	

	

}



//server fire  function only replicates on server ..
void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}


//multicast fire function replicates both on server and client..
void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireWeaponMontage(bAiming);

		//call fire function from weapon class, which has spawn projectile functionality.. 
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (bAiming)
	{
		//while aiming set current FOV to weapon's ZoomedFOV and Interp speed
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(),DeltaTime,  EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		//while not zooming in setting values for all weapon same..
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr) return;

	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues(); //Updating ammo values after reloading animation is completed..
	}

	if (bFireButtonPressed)
	{
		Fire();
	}
}





