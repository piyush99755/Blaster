// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	
}


void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();

	FFramePackage Package;

	SaveFramePackage(Package);
	ShowFramePackage(Package, FColor::Green);
	
}


void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (FrameHistory.Num() <= 1)
	{
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);//adding head element to frame history
	}
	else
	{
		 float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			//when history length is greater than max record time, keep removing node
			//then save that frame history once it get back under max recordtime
			FrameHistory.RemoveNode(FrameHistory.GetTail());
		}
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
		ShowFramePackage(ThisFrame, FColor::Red);
	}

	
}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& Package)
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : Character;

	//getting server time, which is the right one
	Package.Time = GetWorld()->GetDeltaSeconds();

	//using TMap to get theright box component  and getting its location, rotation and box extent
	for (auto& BoxPair : Character->HitBoxCollisions)
	{
		FBoxInformation BoxInformation;
		BoxInformation.BoxLocation = BoxPair.Value->GetComponentLocation();
		BoxInformation.BoxRotation = BoxPair.Value->GetComponentRotation();
		BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();

		//storing this key, value pair in FFramePackage which require hit box info and time
		//adding key as hit box FName and value as BoxInformation
		Package.HitBoxInfo.Add(BoxPair.Key, BoxInformation);
	}
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& Package, const FColor& Color)
{
	for (auto& BoxInfo : Package.HitBoxInfo)
	{
		DrawDebugBox(GetWorld(), BoxInfo.Value.BoxLocation, BoxInfo.Value.BoxExtent, FQuat(BoxInfo.Value.BoxRotation), Color, false, 4.f);

	}
}

