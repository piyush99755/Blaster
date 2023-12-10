// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
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

protected:
	
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	




public:

	

	//function to determine whether or not display pickup widget..
	void ShowPickupWidget(bool bShowWidget);

	FORCEINLINE USphereComponent* GetWeaponSphere() const { return WeaponSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	
	void SetWeaponStatus(EWeaponState State);

	virtual void Fire(const FVector& HitTarget);



};
