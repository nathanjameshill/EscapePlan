// Copyright Original Skain Experiences 2019.

#include "Grabber.h"
#include "Engine/World.h"
#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputCommonent();

}

void UGrabber::Grab() {
	///try find actor with physics body set
	if (!PhysicsHandlePtr) { return; }

	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrapPtr = HitResult.GetComponent();
	auto ActorHitPtr = HitResult.GetActor();

	if (!ActorHitPtr && !ComponentToGrapPtr) { return; }

	PhysicsHandlePtr->GrabComponent(
		ComponentToGrapPtr,
		NAME_None,
		ComponentToGrapPtr->GetOwner()->GetActorLocation(),
		true
	);
}

void UGrabber::Release() {
	///Release physics handle
	if (!PhysicsHandlePtr) { return; }
	PhysicsHandlePtr->ReleaseComponent();

}

void UGrabber::SetupInputCommonent() {

	InputComponentPtr = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponentPtr)
	{
		InputComponentPtr->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponentPtr->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing Input Component"), *GetOwner()->GetName())
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	///Raycast out to reach
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	FHitResult LineTraceHit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT LineTraceHit,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	return LineTraceHit;
}

void UGrabber::FindPhysicsHandleComponent()
{
	///Look for attached physics handle
	PhysicsHandlePtr = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandlePtr == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handler"), *GetOwner()->GetName());
	}
}



// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!PhysicsHandlePtr) { return; }
	if (PhysicsHandlePtr->GrabbedComponent) {
		auto LineTraceEnd = GetReachLineEnd();
		PhysicsHandlePtr->SetTargetLocation(LineTraceEnd);
	}
}


FVector UGrabber::GetReachLineStart() {
	//Get player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
	return PlayerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd() {
	//Get player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

