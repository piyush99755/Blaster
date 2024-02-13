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

void ULagCompensationComponent::ServerSideRewind(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
{
	bool bReturn = HitCharacter ||
		           HitCharacter->GetLagCompensation() ||
		           HitCharacter->GetLagCompensation()->FrameHistory.GetHead() ||
	               HitCharacter->GetLagCompensation()->FrameHistory.GetHead();

	FFramePackage FrameToCheck;
	bool bShouldInterpolate = true;

	//hit character frame history which will be filled in by function 
	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensation()->FrameHistory;
	const float OldestHistoryTime = History.GetTail()->GetValue().Time;
	const float NewestHistoryTime = History.GetHead()->GetValue().Time;

	if (OldestHistoryTime > HitTime)
	{
		return;
	}

	if (OldestHistoryTime == HitTime)
	{
		FrameToCheck = History.GetTail()->GetValue();
		bShouldInterpolate = false;
	}

	if (NewestHistoryTime <= HitTime)
	{
		FrameToCheck = History.GetHead()->GetValue();
		bShouldInterpolate = false;
	}

	//setting both node on to the head node before going through all the frames in frame package..
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;

	while (Older->GetValue().Time > HitTime) //is older is still younger than hit time?
	{
		//march back until OlderTime < HitTime < YoungerTime..
		if (Older->GetNextNode() == nullptr) break;
		Older->GetNextNode()->GetValue();

		if (Older->GetNextNode()->GetValue().Time > HitTime)
		{
			Younger = Older;
		}
	}

	if (Older->GetValue().Time == HitTime)
	{
		FrameToCheck = Older->GetValue();
		bShouldInterpolate = false;
	}

	if (bShouldInterpolate)
	{

	}

	if (bReturn) return;
}

FFramePackage ULagCompensationComponent::InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime)
{
	const float Distance = YoungerFrame.Time - OlderFrame.Time;
	const float InterpFraction = (HitTime - OlderFrame.Time) / Distance;

	FFramePackage InterpFramePackage; 
	InterpFramePackage.Time = HitTime;

	for (auto& YoungerPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxInfoName = YoungerPair.Key;

		const FBoxInformation& OlderBox = OlderFrame.HitBoxInfo[BoxInfoName];
		const FBoxInformation& YoungerBox = YoungerFrame.HitBoxInfo[BoxInfoName];

		FBoxInformation InterpBoxInfo; 

		InterpBoxInfo.BoxLocation = FMath::VInterpTo(OlderBox.BoxLocation, YoungerBox.BoxLocation, 1.f, InterpFraction);
		InterpBoxInfo.BoxRotation = FMath::RInterpTo(OlderBox.BoxRotation, OlderBox.BoxRotation, 1.f, InterpFraction);
		InterpBoxInfo.BoxExtent = YoungerBox.BoxExtent;

		InterpFramePackage.HitBoxInfo.Add(BoxInfoName, InterpBoxInfo);
	}

	return InterpFramePackage;
}

