// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "EquippedSecondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScanWeapon UMETA(DisplayName = "Hit scan Weapon"),
	EFT_ProjectileWeapon UMETA(DisplayName = "Projectile Weapon"),

	EFT_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AWeapon();

	virtual void Tick(float DeltaTime) override;

	//texture variables for weapon crosshairs
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bAutomatic = true;

	bool bWeaponDestroy = false;

	UPROPERTY(EditAnywhere)
		EFireType FireType;

	//trace end with scatter variables.. 
	UPROPERTY(EditAnywhere)
		float DistaceToSphere = 800.f;

	UPROPERTY(EditAnywhere)
		float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere)
		bool bUseScatter = false;

	

protected:
	
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_Owner() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* WeaponSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere)
		EWeaponState WeaponState;

	//rep notify function to handle certain conditions when value of weapon state changes...
	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* PickupWidget;

	//variable for animation asset 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAnimationAsset* FireAnimation;

	//for weapons to choose different type of casing class 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ACasing> CasingClass;

	//zoom in variables for different type of weapons, declaring in base class , which will be available for all child classes

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  meta = (AllowPrivateAccess = "true"))
		float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float ZoomInterpSpeed = 20.f;
	   

	//ammo variables and functions

	UPROPERTY(EditAnywhere)
	int32 Ammo;

	

	void SpendRound();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;
	
	//the number of unprocessed server requests for ammo
	//incremented in spend round and decremented in clientupdateammo
	UPROPERTY()
	int32 Sequence = 0;

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;

	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerController;

	UPROPERTY(EditAnywhere)
		EWeaponType WeaponType; 

	UPROPERTY(EditAnywhere)
	 class USoundCue* EquipSound;

public:

	

	//function to determine whether or not display pickup widget..
	void ShowPickupWidget(bool bShowWidget);

	FORCEINLINE USphereComponent* GetWeaponSphere() const { return WeaponSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const  { return ZoomInterpSpeed; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }

	
	void SetWeaponStatus(EWeaponState State);

	void OnWeaponStateSet();

	void OnEquipped();


	void OnEquippedSecondary();

	void OnDropped();

	virtual void Fire(const FVector& HitTarget);

	void Dropped();

	void SetHUDAmmo();

	bool IsEmpty();

	void AddAmmo(int32 AmmoToAdd);

	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);


	void EnableCustomDepth(bool bEnable);

	FVector TraceEndWithScatter(const FVector& HitTarget);


};
