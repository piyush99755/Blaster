// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


ACasing::ACasing()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	CasingMesh->SetupAttachment(GetRootComponent());

	//collision and physics properties of CasingMesh..
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	CasingMesh->SetEnableGravity(true);

	ShellImpulse = 10.f;



}


void ACasing::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);

	CasingMesh->AddImpulse(GetActorForwardVector() * ShellImpulse);
	
}




void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACasing::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	Destroy();
}

