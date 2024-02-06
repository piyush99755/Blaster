// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Casing.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

AWeapon::AWeapon()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	//in order for weapon to have replication property set this bool to true..
	bReplicates = true;
	SetReplicateMovement(true);

	//constructing weapon mesh 
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

    //setting up its initial collision properties
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	//Constructing weapon sphere
	WeaponSphere = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponSphere"));
	WeaponSphere->SetupAttachment(GetRootComponent());

	//setting up its initial collision properties
	WeaponSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	

	//creating widget component 
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());
	




}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//showing widget locally
	WeaponSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	WeaponSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	WeaponSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	

    if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	
	
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, Ammo);
}



void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);

		if (BlasterCharacter)
		{
			BlasterCharacter->SetOverlappingWeapon(this);

		}
	}
	

	
	

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);


		if (BlasterCharacter)
		{
			BlasterCharacter->SetOverlappingWeapon(nullptr);

		}
	}
	
}

void AWeapon::SetWeaponStatus(EWeaponState State)
{
	WeaponState = State;

	OnWeaponStateSet();
}

void AWeapon::OnWeaponStateSet()
{

	//define properties of different states of weapon
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		OnEquipped();
		break;

	case EWeaponState::EWS_EquippedSecondary:
		OnEquippedSecondary();
		break;

	case EWeaponState::EWS_Dropped:
		OnDropped();
		break;
	}

}

void AWeapon::OnEquipped()
{
	ShowPickupWidget(false);
	WeaponSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//setting up weapon properties once weapon is equipped 
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (WeaponType == EWeaponType::EWT_SubmachineGun)
	{
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	}
	EnableCustomDepth(false);
}

void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeapon::OnEquippedSecondary()
{
	ShowPickupWidget(false);
	WeaponSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//setting up weapon properties once weapon is equipped 
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (WeaponType == EWeaponType::EWT_SubmachineGun)
	{
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	}
	EnableCustomDepth(true);
	
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
	WeaponMesh->MarkRenderStateDirty();
}

void AWeapon::OnDropped()
{
	if (HasAuthority())
	{
		WeaponSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	//setting simulate physcis and gravity to true when weapon is in dropped state..
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);
}



void AWeapon::Dropped()
{
	SetWeaponStatus(EWeaponState::EWS_Dropped);

	//detaching weapon when weapon gets into dropped state..
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterOwnerController = nullptr;
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}

	//spawning casing such as bullet shells using weapon socket and its transform..
	if (CasingClass)
	{
		

		//getting socket attached to weapon mesh
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);

	
            UWorld* World = GetWorld();
			if (World)
			{
			   World->SpawnActor<ACasing>(CasingClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
			}
			
		}
	}
	if (HasAuthority())
	{
		SpendRound();
	}
	
}

void AWeapon::SetHUDAmmo()
{
	//setting and updating  weapon ammo hud on every round spent...
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if (BlasterOwnerCharacter)
	{
		BlasterOwnerController = BlasterOwnerController == nullptr ? Cast <ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
		if (BlasterOwnerController)
		{
			BlasterOwnerController->SetWeaponAmmoHUD(Ammo);
		}
	}
}


void AWeapon::SpendRound()
{
	//stop ammo from firing at value of 0
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);

	SetHUDAmmo();

}

bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	//clamping value to make sure that value doesnot go below 0 and above mag capacity
	Ammo = FMath::Clamp(Ammo - AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
}

void AWeapon::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}



void AWeapon::OnRep_Ammo()
{
	SetHUDAmmo();
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (Owner == nullptr)
	{
		BlasterOwnerCharacter = nullptr;
		BlasterOwnerController = nullptr;
	}
	else
	{
		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(Owner) : BlasterOwnerCharacter;
		if (BlasterOwnerCharacter && BlasterOwnerCharacter->GetEquippedWeapon() && BlasterOwnerCharacter->GetEquippedWeapon() == this)
		{
			SetHUDAmmo();
		}
		
	}
	
	
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}



